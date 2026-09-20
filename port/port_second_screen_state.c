#include "port_second_screen_state.h"

#include <string.h>

#if defined(__ANDROID__) || defined(TMC_3DS)

#ifdef __ANDROID__
#include <pthread.h>
#define SNAPSHOT_MUTEX_TYPE pthread_mutex_t
#define SNAPSHOT_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define SNAPSHOT_MUTEX_LOCK(m) pthread_mutex_lock(m)
#define SNAPSHOT_MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#else
#include "port_second_screen_sync_3ds.h"
#define SNAPSHOT_MUTEX_TYPE unsigned char
#define SNAPSHOT_MUTEX_INITIALIZER 0
#define SNAPSHOT_MUTEX_LOCK(m) Port_SecondScreen_3DS_LockSnapshot()
#define SNAPSHOT_MUTEX_UNLOCK(m) Port_SecondScreen_3DS_UnlockSnapshot()
#endif

#include "area.h"
#include "common.h"
#include "entity.h"
#include "flags.h"
#include "game.h"
#include "item.h"
#include "itemMetaData.h"
#include "kinstone.h"
#include "main.h"
#include "player.h"
#include "room.h"
#include "save.h"
#include "subtask.h" /* sub_080A6F40 — the map screens' hint-visibility word */
#include "ui.h"
#include "object.h"
#include "port_runtime_config.h"
#include "rando/rando.h"

/* The HUD's per-language button-label frame offsets (data/const/ui.s),
 * declared exactly as src/ui.c declares them. */
extern const u8 gUnk_080C9044[];

/* gItemMetaData menu slot the quest screen treats as "a carried quest
 * item" rather than a well of its own: sub_080A5594 spreads every item
 * marked with it across three slots instead of placing it directly. */
#define QUEST_CARRIED_ITEM_SLOT 1

static SecondScreenSnapshot sSnapshot;
static SNAPSHOT_MUTEX_TYPE sSnapshotMutex = SNAPSHOT_MUTEX_INITIALIZER;

/* Pending tap-to-equip request from the UI thread, consumed by Publish().
 * itemId 0 = nothing pending. Guarded by sSnapshotMutex — same one-memcpy
 * discipline, never held across engine calls. */
static uint8_t sPendingEquipItem = 0;
static uint8_t sPendingEquipSlot = 0;
static SecondScreenResource sPendingResource = SECOND_SCREEN_RESOURCE_NONE;

/* Story-golden Kinstones are bag types 0x65..0x6d, not fusion ids 1..9.
 * Each gold shape is shared by one or two of the first nine story fusions.
 * A gold piece remains useful while at least one matching fusion is pending;
 * otherwise it must disappear instead of becoming a stack of 99. */
static bool GoldenKinstoneStillNeeded(uint8_t type) {
    switch (type) {
        case 0x65: return !CheckKinstoneFused(1);
        case 0x66:
        case 0x69: return !CheckKinstoneFused(2) || !CheckKinstoneFused(5);
        case 0x67:
        case 0x68: return !CheckKinstoneFused(3) || !CheckKinstoneFused(4);
        case 0x6a: return !CheckKinstoneFused(6);
        case 0x6b: return !CheckKinstoneFused(7);
        case 0x6c: return !CheckKinstoneFused(8);
        case 0x6d: return !CheckKinstoneFused(9);
        default: return false;
    }
}

static void RefillKinstoneBagSafely(void) {
    KinstoneSave* bag = &gSave.kinstones;
    uint32_t write = 0;

    for (uint32_t read = 0; read < 19; read++) {
        const uint8_t type = bag->types[read];
        uint8_t amount = bag->amounts[read];
        if (type == KINSTONE_NONE || amount == 0) continue;

        if (type >= 0x65 && type <= 0x6d) {
            if (!GoldenKinstoneStillNeeded(type)) continue;
            amount = 1;
        } else {
            amount = 99;
        }

        bag->types[write] = type;
        bag->amounts[write] = amount;
        write++;
    }

    while (write < 19) {
        bag->types[write] = KINSTONE_NONE;
        bag->amounts[write] = 0;
        write++;
    }
}

/* Port-side automap: which rooms of each area have been entered this
 * session. TMC's own per-room "visited" state is scattered across
 * area-specific local flags with no uniform room->flag mapping, so the
 * port tracks it directly — same approach as zelda3-android's visited-room
 * dungeon map. Game-thread only. */
static uint64_t sVisitedByArea[256];

typedef struct {
    uint8_t area;
    uint16_t flag;
    int16_t x;
    int16_t y;
} HeartMapLocation;

/* Build a world-map locator from the ROM's room entity lists.  Unlike
 * the live-object attempt, this sees heart pieces before Link enters their
 * room.  ItemOnGround receives its permanent collection flag from the high
 * half of EntityData.spritePtr (RegisterRoomEntity). */
static HeartMapLocation sHeartMapLocations[SECOND_SCREEN_HEART_MARKERS];
static uint8_t sHeartMapLocationCount;
static bool sHeartMapScanned;

static void AddHeartMapLocations(uint32_t area, const RoomHeader* header,
                                 const EntityData* data) {
    for (uint32_t n = 0; data != NULL && n < 192 && data[n].kind != 0xff; n++) {
        const EntityData* e = &data[n];
        const uint16_t flag = (uint16_t)(e->spritePtr >> 16);
        if ((e->kind & 0x0f) != OBJECT || e->id != GROUND_ITEM ||
            e->type != ITEM_HEART_PIECE || flag == 0) {
            continue;
        }

        /* A room can expose the same list through both properties.  Keep a
         * single pin in that case, but do not discard property 0 outright:
         * the normal room loader consumes both property 1 and property 0,
         * and several real overworld heart pieces live in the latter. */
        /* Reject an EntityData-shaped value from a non-entity property list.
         * A real placed item must sit within its room's pixel rectangle. */
        if (e->xPos >= header->pixel_width || e->yPos >= header->pixel_height) {
            continue;
        }

        /* Both values are already in world pixels. SetWorldMapPos() uses
         * exactly header->map_x + local_x for the game's own map cursor.
         * Shifting map_x/map_y here wrapped large overworld coordinates and
         * produced pins in unrelated regions. */
        const int16_t x = (int16_t)((int32_t)header->map_x + e->xPos);
        const int16_t y = (int16_t)((int32_t)header->map_y + e->yPos);
        bool duplicate = false;
        for (uint32_t i = 0; i < sHeartMapLocationCount; i++) {
            const HeartMapLocation* prior = &sHeartMapLocations[i];
            if (prior->area == area && prior->flag == flag &&
                prior->x == x && prior->y == y) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate && sHeartMapLocationCount < SECOND_SCREEN_HEART_MARKERS) {
            HeartMapLocation* out = &sHeartMapLocations[sHeartMapLocationCount++];
            out->area = (uint8_t)area;
            out->flag = flag;
            out->x = x;
            out->y = y;
        }
    }
}

/* EntityData.spritePtr's upper half becomes ItemOnGroundEntity.flag.  It is
 * an encoded CheckFlags value, not invariably a local-flag ordinal.  The
 * scanner must therefore decode it using the source area's bank rather than
 * applying the current area's local-bank shortcut.  Room flags are volatile
 * by definition, so they cannot honestly power a persistent world-map hint. */
static bool HeartMapLocationWasCollected(const HeartMapLocation* heart) {
    const uint32_t flag = heart->flag;
    const uint32_t index = flag & 0x3ffu;
    const uint32_t type = (flag >> 14) & 3u;
    const uint8_t bank = gAreaMetadata[heart->area].flag_bank;

    switch (type) {
        case 0: /* local flag in the area that owns the item */
            /* The location tables are authored against the USA ROM.  A
             * translated ROM keeps the same gameplay flags but may move the
             * local-flag ordinals; use the region-aware reader just like the
             * item scripts do. */
            return bank <= LOCAL_BANK_12 && CheckLocalFlagByBankB(gLocalFlagBanks[bank], index);
        case 1: /* global flag */
            return CheckGlobalFlag(index);
        case 2: /* room flag: not saved outside the currently loaded room */
            return true;
        default:
            return true;
    }
}

static void BuildHeartMapLocations(void) {
    if (sHeartMapScanned || gRomData == NULL || gAreaRoomHeaders == NULL) return;
    for (uint32_t area = 0; area < 153 && sHeartMapLocationCount < SECOND_SCREEN_HEART_MARKERS; area++) {
        const RoomHeader* headers;
        /* This is intentionally not limited to AR_IS_OVERWORLD: most of the
         * canonical 44 live in caves, houses or dungeons.  The publisher
         * below filters world-map pins; the diagnostic must see all of them. */
        headers = gAreaRoomHeaders[area];
        if (headers == NULL) continue;
        for (uint32_t room = 0; room < MAX_ROOMS && headers[room].map_x != 0xffffu &&
                             sHeartMapLocationCount < SECOND_SCREEN_HEART_MARKERS; room++) {
            /* The normal room loader consumes both lists.  Heart pieces in
             * either one must be discoverable before Link visits the room. */
            AddHeartMapLocations(area, &headers[room],
                                 (const EntityData*)GetRoomProperty(area, room, 1));
            if (sHeartMapLocationCount < SECOND_SCREEN_HEART_MARKERS) {
                AddHeartMapLocations(area, &headers[room],
                                     (const EntityData*)GetRoomProperty(area, room, 0));
            }
        }
    }
    sHeartMapScanned = true;
}

static void PublishOutdoorHeartMarkers(SecondScreenSnapshot* next) {
    BuildHeartMapLocations();
    if (next == NULL || !Port_Config_GetHeartMapMarkers()) return;
    for (uint32_t i = 0; i < sHeartMapLocationCount && next->heartMarkerCount < SECOND_SCREEN_HEART_MARKERS; i++) {
        const HeartMapLocation* heart = &sHeartMapLocations[i];
        /* Interior and dungeon coordinates belong to their own maps.  Do not
         * project them onto the overworld parchment: that was the source of
         * the seemingly random hearts the player reported. */
        if ((gAreaMetadata[heart->area].flags & AR_IS_OVERWORLD) == 0) continue;
        if (HeartMapLocationWasCollected(heart)) continue;
        next->heartMarkerArea[next->heartMarkerCount] = heart->area;
        next->heartMarkerX[next->heartMarkerCount] = heart->x;
        next->heartMarkerY[next->heartMarkerCount] = heart->y;
        next->heartMarkerCount++;
    }
}

unsigned Port_SecondScreenState_GetMissingHeartPieces(PortHeartMapReportEntry* entries,
                                                       unsigned capacity) {
    unsigned count = 0;

    /* Do not use the randomizer's USA location-key catalog as a completion
     * oracle here. Text-patched base-USA ROMs may relocate local flag
     * ordinals without changing their game code, making that catalog report
     * already-collected chest rewards as missing. The room entity data is
     * loaded from the active ROM and carries the collection flag actually
     * used by each placed heart, so it is the safe source for a player-facing
     * PT-BR diagnostic. */
    BuildHeartMapLocations();
    for (uint32_t i = 0; i < sHeartMapLocationCount; ++i) {
        const HeartMapLocation* heart = &sHeartMapLocations[i];
        if (HeartMapLocationWasCollected(heart)) continue;
        if (entries != NULL && count < capacity) {
            entries[count].area = heart->area;
            entries[count].flag = heart->flag;
            entries[count].x = heart->x;
            entries[count].y = heart->y;
            entries[count].name = "Placed heart piece";
            entries[count].source = 2;
        }
        ++count;
    }
    return count;
}

void Port_SecondScreenState_Publish(void) {
    /* Assembled outside the lock: this runs on the game thread itself, the
     * same thread that owns gRoomControls/gPlayerEntity/gSave during normal
     * gameplay, so reading them here is exactly as safe as any other engine
     * code doing so — no cross-thread race on this side. The lock only
     * needs to guard the swap into sSnapshot, which the second-screen
     * thread does read cross-thread. */
    SecondScreenSnapshot next;
    memset(&next, 0, sizeof(next));

    uint8_t equipItem;
    uint8_t equipSlot;
    SecondScreenResource resource;
    SNAPSHOT_MUTEX_LOCK(&sSnapshotMutex);
    equipItem = sPendingEquipItem;
    equipSlot = sPendingEquipSlot;
    resource = sPendingResource;
    sPendingEquipItem = 0;
    sPendingResource = SECOND_SCREEN_RESOURCE_NONE;
    SNAPSHOT_MUTEX_UNLOCK(&sSnapshotMutex);

    next.inGame = gMain.task == TASK_GAME;
    if (next.inGame) {
        /* Tap-to-equip goes through the engine's own path (swap handling,
         * HUD refresh) and only for items actually in the inventory — a
         * stale tap from a previous save can't equip something Link
         * doesn't own. */
        if (equipItem != 0 && GetInventoryValue(equipItem) == 1) {
            ForceEquipItem(equipItem, equipSlot ? EQUIP_SLOT_B : EQUIP_SLOT_A);
        }

        switch (resource) {
            case SECOND_SCREEN_RESOURCE_HEARTS:
                gSave.stats.health = gSave.stats.maxHealth;
                break;
            case SECOND_SCREEN_RESOURCE_RUPEES:
                gSave.stats.rupees = 999;
                break;
            case SECOND_SCREEN_RESOURCE_SHELLS:
                if (GetInventoryValue(ITEM_SHELLS) != 0) gSave.stats.shells = 999;
                break;
            case SECOND_SCREEN_RESOURCE_KINSTONES:
                if (GetInventoryValue(ITEM_KINSTONE_BAG) != 0) {
                    RefillKinstoneBagSafely();
                }
                break;
            case SECOND_SCREEN_RESOURCE_BOMBS:
                if (GetInventoryValue(ITEM_BOMBS) != 0)
                    gSave.stats.bombCount = gBombBagSizes[gSave.stats.bombBagType & 3];
                break;
            case SECOND_SCREEN_RESOURCE_ARROWS:
                if (GetInventoryValue(ITEM_BOW) != 0)
                    gSave.stats.arrowCount = gQuiverSizes[gSave.stats.quiverType & 3];
                break;
            default:
                break;
        }

        next.area = gRoomControls.area;
        next.room = gRoomControls.room;
        next.playerX = gPlayerEntity.base.x.HALF.HI;
        next.playerY = gPlayerEntity.base.y.HALF.HI;
        next.equippedA = gSave.stats.equipped[SLOT_A];
        next.equippedB = gSave.stats.equipped[SLOT_B];
        next.equippedSlotA = next.equippedA ? gItemMetaData[next.equippedA].menuSlot : 0xFF;
        next.equippedSlotB = next.equippedB ? gItemMetaData[next.equippedB].menuSlot : 0xFF;
        next.health = gSave.stats.health;
        next.maxHealth = gSave.stats.maxHealth;
        next.topHudHidden = Port_Config_GetHideTopHud() ? 1 : 0;
        next.chargeAction = gPlayerState.chargeState.action;
        next.chargeTimer = gPlayerState.chargeState.chargeTimer;
        next.rupees = gSave.stats.rupees;

        /* Area identity + per-dungeon save state, gated exactly like the
         * engine gates it: dungeonKeys/dungeonItems are only meaningful
         * where AreaHasKeys() holds (src/gameUtils.c reads them through
         * gArea.dungeon_idx under that same check). */
        next.areaFlags = gArea.areaMetadata;
        next.dungeonIdx = gArea.dungeon_idx;
        if ((next.areaFlags & SECOND_SCREEN_AR_HAS_KEYS) && next.dungeonIdx < 0x10) {
            next.dungeonKeys = gSave.dungeonKeys[next.dungeonIdx];
            next.dungeonItemBits = gSave.dungeonItems[next.dungeonIdx];
        }

        /* Quest state for the status strip — plain gSave field reads. */
        for (u32 i = 0; i < 4; i++) {
            if (GetInventoryValue(ITEM_EARTH_ELEMENT + i) == 1) {
                next.elements |= (uint8_t)(1u << i);
            }
        }
        next.walletType = gSave.stats.walletType & 3;
        next.walletMax = gWalletSizes[next.walletType].size;
        next.bombCount = gSave.stats.bombCount;
        next.bombMax = gBombBagSizes[gSave.stats.bombBagType & 3];
        next.arrowCount = gSave.stats.arrowCount;
        next.arrowMax = gQuiverSizes[gSave.stats.quiverType & 3];
        next.kinstoneFused = gSave.kinstones.fusedCount;
        for (u32 i = 0; i < 19; i++) {
            next.kinstoneBag += gSave.kinstones.amounts[i];
        }

        /* QUEST STATUS screen values, read exactly where sub_080A5594
         * (src/menu/pauseMenu.c) reads them when it fills that screen's
         * sixteen slots. Plain gSave and GetInventoryValue reads, no
         * different in cost or safety from the ones above. */
        next.heartPieces = gSave.stats.heartPieces;
        for (u32 i = ITEM_SKILL_SPIN_ATTACK; i <= ITEM_SKILL_PERIL_BEAM; i++) {
            if (GetInventoryValue(i) != 0) {
                next.swordSkills++;
                next.swordSkillBits |= (uint8_t)(1u << (i - ITEM_SKILL_SPIN_ATTACK));
            }
        }
        /* The bag rows verbatim, for the pieces list behind the bag's well. */
        for (u32 i = 0; i < 19; i++) {
            next.kinstoneTypes[i] = gSave.kinstones.types[i];
            next.kinstoneAmounts[i] = gSave.kinstones.amounts[i];
        }
        next.shells = gSave.stats.shells;
        next.shellsOwned = (uint8_t)GetInventoryValue(ITEM_SHELLS);
        next.carlovMedal = (uint8_t)GetInventoryValue(ITEM_QST_CARLOV_MEDAL);
        next.tingleTrophy = (uint8_t)GetInventoryValue(ITEM_QST_TINGLE_TROPHY);
        next.kinstoneBagOwned = GetInventoryValue(ITEM_KINSTONE_BAG) != 0;
        for (u32 i = 0; i < 3; i++) {
            if (GetInventoryValue(ITEM_GRIP_RING + i) == 1) {
                next.passives |= (uint8_t)(1u << i);
            }
        }
        /* The carried-item tray, filled the way sub_080A5594's rolling
         * counter fills slots 6..8: every quest item whose metadata puts
         * it in the shared carried-item slot queues up in item-id order,
         * and once the tray is full the last position keeps whichever is
         * last owned. Items handed in (inventory value 2) drop out of the
         * tray, which is why the test is == 1 and not != 0. */
        {
            u32 tray = 0;
            for (u32 item = ITEM_QST_SWORD; item <= ITEM_FLIPPERS; item++) {
                if (gItemMetaData[item].menuSlot != QUEST_CARRIED_ITEM_SLOT ||
                    GetInventoryValue(item) != 1) {
                    continue;
                }
                next.questItems[tray] = (uint8_t)item;
                if (tray < 2) {
                    tray++;
                }
            }
        }
        {
            /* Owned figurines = set bits in the save's figurine bitset. */
            u32 n = 0;
            for (u32 i = 0; i < sizeof(gSave.figurines); i++) {
                u8 b = gSave.figurines[i];
                while (b) {
                    n += b & 1;
                    b >>= 1;
                }
            }
            next.figurineCount = n > 255 ? 255 : (uint8_t)n;
        }
        next.windcrests = gSave.windcrests;
        memcpy(next.fusedKinstones, gSave.kinstones.fusedKinstones, sizeof(next.fusedKinstones));
        memcpy(next.fusionUnmarked, gSave.kinstones.fusionUnmarked, sizeof(next.fusionUnmarked));

        /* Map hints, resolved the way both map screens resolve them:
         * `gSave.map_hints & sub_080A6F40()` (src/menu/pauseMenu.c
         * sub_080A6438, src/menu/pauseMenuScreen6.c sub_080A68D4). The
         * second operand walks gUnk_08128F38's (type, arg) pairs and clears
         * a hint's bit once sub_0807CB24 says its errand is done — local
         * flags and inventory, i.e. exactly the live save state the
         * second-screen render thread must not touch. The engine's own
         * function is called rather than re-derived: it is a pure predicate
         * (reads gSave, writes nothing), and a copy here would be one more
         * place to keep in step with the flag-bank remaps sub_0807CB24
         * applies on EU/JP. The real menu caches the same value on screen
         * entry (sub_080A6290); publishing per tick is only fresher. */
        next.mapHints = (uint16_t)(gSave.map_hints & sub_080A6F40());
        PublishOutdoorHeartMarkers(&next);

        /* Contextual R prompt, resolved exactly like TextUIElement's
         * type2 == 9 branch (src/ui.c): the player-state action wins, else
         * the area's portal mode names the shrink/grow prompt, else the
         * interactable under Link. The frame id the HUD would actually
         * stamp is that value plus the language's label-block offset —
         * publish both, since the raw action is what names the prompt when
         * the label art isn't available. */
        {
            u32 rAction = gHUD.rActionPlayerState;
            if (rAction == R_ACTION_NONE) {
                switch (gArea.portal_mode) {
                    case 2:
                        rAction = R_ACTION_SHRINK;
                        break;
                    case 3:
                        rAction = R_ACTION_GROW;
                        break;
                    default:
                        rAction = gHUD.rActionInteractObject;
                        break;
                }
            }
            next.rActionId = (uint8_t)rAction;
            if (rAction != 0) {
                rAction += gUnk_080C9044[gSaveHeader->language];
            }
            next.rActionFrame = (uint8_t)rAction;
        }

        /* Mirror of the pause menu's item-screen fill loop
         * (src/menu/pauseMenu.c: PauseMenu_ItemMenu_Init): every owned
         * activatable item lands in its ItemMetaData menu slot; later item
         * ids overwrite earlier ones in the same slot, exactly like the
         * real menu. */
        for (u32 item = ITEM_SMITH_SWORD; item < ITEM_BOTTLE_EMPTY; item++) {
            if (GetInventoryValue(item) == 1) {
                u32 slot = gItemMetaData[item].menuSlot;
                if (slot < SECOND_SCREEN_ITEM_SLOTS) {
                    next.menuItems[slot] = (uint8_t)item;
                }
            }
        }
        for (u32 i = 0; i < 4; i++) {
            next.bottleContents[i] = gSave.stats.bottles[i];
        }

        for (u32 i = 0; i < SECOND_SCREEN_MAX_ROOMS && i < MAX_ROOMS; i++) {
            const RoomResInfo* info = &gArea.roomResInfos[i];
            next.rooms[i].x = info->map_x;
            next.rooms[i].y = info->map_y;
            next.rooms[i].w = info->pixel_width;
            next.rooms[i].h = info->pixel_height;
        }

        sVisitedByArea[next.area] |= 1ull << (next.room & 63);
        next.visitedMask = sVisitedByArea[next.area];
    }

    SNAPSHOT_MUTEX_LOCK(&sSnapshotMutex);
    sSnapshot = next;
    SNAPSHOT_MUTEX_UNLOCK(&sSnapshotMutex);
}

void Port_SecondScreenState_Read(SecondScreenSnapshot* out) {
    SNAPSHOT_MUTEX_LOCK(&sSnapshotMutex);
    *out = sSnapshot;
    SNAPSHOT_MUTEX_UNLOCK(&sSnapshotMutex);
}

void Port_SecondScreenState_RequestEquip(uint8_t itemId, uint8_t slot) {
    SNAPSHOT_MUTEX_LOCK(&sSnapshotMutex);
    sPendingEquipItem = itemId;
    sPendingEquipSlot = slot;
    SNAPSHOT_MUTEX_UNLOCK(&sSnapshotMutex);
}

void Port_SecondScreenState_RequestResource(SecondScreenResource resource) {
    SNAPSHOT_MUTEX_LOCK(&sSnapshotMutex);
    sPendingResource = resource;
    SNAPSHOT_MUTEX_UNLOCK(&sSnapshotMutex);
}

#else /* Platforms without a live second-screen state consumer. */

void Port_SecondScreenState_Publish(void) {}

void Port_SecondScreenState_Read(SecondScreenSnapshot* out) {
    memset(out, 0, sizeof(*out));
}

void Port_SecondScreenState_RequestEquip(uint8_t itemId, uint8_t slot) {
    (void)itemId;
    (void)slot;
}

void Port_SecondScreenState_RequestResource(SecondScreenResource resource) { (void)resource; }

#endif
