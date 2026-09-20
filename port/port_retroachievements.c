#include "port_retroachievements.h"

#include "port_rom.h"
#include "port_runtime_config.h"

#include "area.h"
#include "main.h"
#include "player.h"
#include "room.h"
#include "save.h"

#ifdef TMC_3DS
#include "port_gba_mem.h"
#include "port_retroachievements_3ds.h"
#endif

#include <rc_client.h>
#include <rc_consoles.h>
#include <rc_hash.h>

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The official Minish Cap GBA set currently accepts this clean USA hash.
 * Keep identification local; no credentials, user data, or network traffic
 * is touched until the later authenticated transport is ready. */
static const char kMinishCapUsaHash[] = "a104896da0047abe8bee2a6e3f4c7290";
static char sRomHash[33];
static int sRomSupported;
static int sUsaBaseCompatible;

enum { RA_OFFLINE, RA_SIGNING_IN, RA_LOADING_GAME, RA_ONLINE, RA_ERROR, RA_UNSUPPORTED };
static _Atomic int sState = RA_OFFLINE;
static rc_client_t* sClient;
static char sUserName[64];
static _Atomic uint32_t sPopupGeneration;
static _Atomic int sPopupState = PORT_RETRO_POPUP_NONE;
static atomic_flag sPopupLock = ATOMIC_FLAG_INIT;
static char sPopupBadgeName[32];

enum { RA_ACHIEVEMENT_CAPACITY = 96 };
static PortRetroAchievement sAchievements[RA_ACHIEVEMENT_CAPACITY];
static _Atomic size_t sAchievementCount;
static _Atomic size_t sUnlockedCount;
static size_t sLastObservedUnlocked;

static void QueuePopup(PortRetroPopup popup, const char* badgeName) {
    while (atomic_flag_test_and_set(&sPopupLock)) {}
    snprintf(sPopupBadgeName, sizeof(sPopupBadgeName), "%s", badgeName ? badgeName : "");
    atomic_flag_clear(&sPopupLock);
    atomic_store(&sPopupState, popup);
    atomic_fetch_add(&sPopupGeneration, 1);
}

static void RetroEvent(const rc_client_event_t* event, rc_client_t* client) {
    (void)client;
    if (!event) return;
    if (event->type == RC_CLIENT_EVENT_ACHIEVEMENT_TRIGGERED && event->achievement) {
        QueuePopup(PORT_RETRO_POPUP_ACHIEVEMENT_DETECTED, event->achievement->badge_name);
    } else if (event->type == RC_CLIENT_EVENT_SERVER_ERROR) {
        QueuePopup(PORT_RETRO_POPUP_SUBMISSION_PENDING, NULL);
    } else if (event->type == RC_CLIENT_EVENT_RECONNECTED) {
        QueuePopup(PORT_RETRO_POPUP_SUBMISSION_COMPLETE, NULL);
    }
}

uint32_t Port_RetroAchievements_PopupGeneration(void) {
    return atomic_load(&sPopupGeneration);
}

PortRetroPopup Port_RetroAchievements_PopupState(void) {
    return (PortRetroPopup)atomic_load(&sPopupState);
}

int Port_RetroAchievements_PopupBadgeName(char* out, size_t outSize) {
    if (!out || outSize == 0) return 0;
    while (atomic_flag_test_and_set(&sPopupLock)) {}
    snprintf(out, outSize, "%s", sPopupBadgeName);
    atomic_flag_clear(&sPopupLock);
    return out[0] != '\0';
}

void Port_RetroAchievements_RefreshList(void) {
    rc_client_achievement_list_t* list;
    size_t count = 0;
    size_t unlocked = 0;

    atomic_store(&sAchievementCount, 0);
    atomic_store(&sUnlockedCount, 0);
    memset(sAchievements, 0, sizeof(sAchievements));
    if (!sClient || atomic_load(&sState) != RA_ONLINE) return;

    list = rc_client_create_achievement_list(sClient, RC_CLIENT_ACHIEVEMENT_CATEGORY_CORE,
                                             RC_CLIENT_ACHIEVEMENT_LIST_GROUPING_LOCK_STATE);
    if (!list) return;

    for (uint32_t bucket = 0; bucket < list->num_buckets && count < RA_ACHIEVEMENT_CAPACITY; ++bucket) {
        const rc_client_achievement_bucket_t* group = &list->buckets[bucket];
        for (uint32_t i = 0; i < group->num_achievements && count < RA_ACHIEVEMENT_CAPACITY; ++i) {
            const rc_client_achievement_t* source = group->achievements[i];
            PortRetroAchievement* target = &sAchievements[count++];
            if (!source) {
                --count;
                continue;
            }
            /* rcheevos may prepend an informational warning to its list.
             * It is not part of the official game set and must not consume
             * a page in the player's achievement browser. */
            if (source->title && strncmp(source->title, "Warning:", 8) == 0) {
                --count;
                continue;
            }
            snprintf(target->title, sizeof(target->title), "%s", source->title ? source->title : "SEM TITULO");
            snprintf(target->description, sizeof(target->description), "%s",
                     source->description ? source->description : "");
            snprintf(target->badgeName, sizeof(target->badgeName), "%s",
                     source->badge_name ? source->badge_name : "");
            target->points = source->points;
            target->unlocked = source->unlocked != RC_CLIENT_ACHIEVEMENT_UNLOCKED_NONE;
            unlocked += target->unlocked;
        }
    }
    rc_client_destroy_achievement_list(list);
    /* Keep earned achievements first. It makes the compact 3DS gallery a
     * record of the player's progress instead of burying it after every
     * locked badge returned by the service. Preserve the service order
     * within each of the two groups. */
    for (size_t i = 0; i < count; ++i) {
        if (!sAchievements[i].unlocked) continue;
        for (size_t j = i; j > 0 && !sAchievements[j - 1].unlocked; --j) {
            PortRetroAchievement swap = sAchievements[j - 1];
            sAchievements[j - 1] = sAchievements[j];
            sAchievements[j] = swap;
        }
    }
    atomic_store(&sUnlockedCount, unlocked);
    atomic_store(&sAchievementCount, count);
}

size_t Port_RetroAchievements_Count(void) { return atomic_load(&sAchievementCount); }
size_t Port_RetroAchievements_UnlockedCount(void) { return atomic_load(&sUnlockedCount); }

int Port_RetroAchievements_Get(size_t index, PortRetroAchievement* out) {
    size_t count = atomic_load(&sAchievementCount);
    if (!out || index >= count) return 0;
    *out = sAchievements[index];
    return 1;
}

#ifdef TMC_3DS
/* rcheevos exposes each console through a compact, linear address space.
 * For GBA that is not the physical bus map: 000000-007fff is IWRAM and
 * 008000-047fff is EWRAM.  Passing those offsets directly to gba_TryMemPtr
 * made every achievement read return NULL/zero even though the title and
 * achievement list loaded correctly from the service. */
static uint8_t* ReadRetroGbaAddress(uint32_t address) {
    uint32_t gbaAddress;

    if (address < 0x008000u)
        gbaAddress = 0x03000000u + address;
    else if (address < 0x048000u)
        gbaAddress = 0x02000000u + (address - 0x008000u);
    else
        return NULL; /* GBA cartridge SRAM is not part of the live port state. */

    /* The native port keeps several originally-GBA globals in host memory,
     * rather than as aliases into gEwram/gIwram.  Give the achievement
     * runtime the same byte layout the retail GBA executable had. The most
     * commonly used Minish Cap conditions are save flags, room controls and
     * the player object; without these bridges the correct virtual address
     * still read an unrelated zero-filled backing buffer. */
    if (gbaAddress >= 0x02002A40u && gbaAddress < 0x02002A40u + sizeof(gSave))
        return (uint8_t*)&gSave + (gbaAddress - 0x02002A40u);
    if (gbaAddress >= 0x02033A90u && gbaAddress < 0x02033A90u + sizeof(gArea))
        return (uint8_t*)&gArea + (gbaAddress - 0x02033A90u);
    if (gbaAddress >= 0x02034350u && gbaAddress < 0x02034350u + sizeof(gRoomVars))
        return (uint8_t*)&gRoomVars + (gbaAddress - 0x02034350u);
    if (gbaAddress >= 0x03000BF0u && gbaAddress < 0x03000BF0u + sizeof(gRoomControls))
        return (uint8_t*)&gRoomControls + (gbaAddress - 0x03000BF0u);
    if (gbaAddress >= 0x03001000u && gbaAddress < 0x03001000u + sizeof(gMain))
        return (uint8_t*)&gMain + (gbaAddress - 0x03001000u);
    if (gbaAddress >= 0x030010A0u && gbaAddress < 0x030010A0u + sizeof(gRoomTransition))
        return (uint8_t*)&gRoomTransition + (gbaAddress - 0x030010A0u);
    if (gbaAddress >= 0x03001160u && gbaAddress < 0x03001160u + sizeof(gPlayerEntity))
        return (uint8_t*)&gPlayerEntity + (gbaAddress - 0x03001160u);

    return gba_TryMemPtr(gbaAddress);
}

static uint32_t ReadGbaMemory(uint32_t address, uint8_t* buffer, uint32_t bytes, rc_client_t* client) {
    uint32_t read = 0;
    (void)client;
    while (read < bytes) {
        uint8_t* source = ReadRetroGbaAddress(address + read);
        if (source == NULL) break;
        buffer[read++] = *source;
    }
    return read;
}

typedef struct {
    rc_client_server_callback_t callback;
    void* callbackData;
} RetroServerCallback;

/* HTTPS involves DNS, TLS and a response read. It must not happen inside
 * rc_client_do_frame on the game thread: that was the short hitch/black
 * bottom-screen frame observed when an achievement was awarded. rcheevos'
 * server callback contract is asynchronous, so complete the request from a
 * small detached worker instead. */
static void RetroServerRequestComplete(char* body, size_t bodyLength, int status, int success, void* userdata) {
    RetroServerCallback* pending = (RetroServerCallback*)userdata;
    rc_api_server_response_t response = { NULL, 0, RC_API_SERVER_RESPONSE_CLIENT_ERROR };
    if (success) {
        response.body = body;
        response.body_length = bodyLength;
        response.http_status_code = status;
    }
    if (pending && pending->callback) pending->callback(&response, pending->callbackData);
    free(pending);
}

static void ServerCall(const rc_api_request_t* request, rc_client_server_callback_t callback, void* callbackData,
                       rc_client_t* client) {
    RetroServerCallback* pending;
    (void)client;
    if (!request || !callback) return;
    pending = calloc(1, sizeof(*pending));
    if (pending) {
        pending->callback = callback;
        pending->callbackData = callbackData;
    }
    if (!pending || !Port_RetroAchievements3DS_RequestAsync(request->url, request->post_data,
                                                            request->content_type,
                                                            RetroServerRequestComplete, pending)) {
        rc_api_server_response_t response = { NULL, 0, RC_API_SERVER_RESPONSE_CLIENT_ERROR };
        free(pending);
        callback(&response, callbackData);
    }
}

static void GameLoaded(int result, const char* message, rc_client_t* client, void* userdata) {
    (void)client;
    (void)userdata;
    (void)message;
    atomic_store(&sState, result == RC_OK ? RA_ONLINE : RA_ERROR);
    if (result == RC_OK) {
        Port_RetroAchievements_RefreshList();
        sLastObservedUnlocked = atomic_load(&sUnlockedCount);
        {
            const size_t count = atomic_load(&sAchievementCount);
            const char* names[RA_ACHIEVEMENT_CAPACITY];
            for (size_t i = 0; i < count; ++i) names[i] = sAchievements[i].badgeName;
            Port_RetroAchievements3DS_PrefetchBadges(names, count);
        }
    }
}

static void LoginFinished(int result, const char* message, rc_client_t* client, void* userdata) {
    const rc_client_user_t* user;
    (void)message;
    (void)userdata;
    if (result != RC_OK) {
        atomic_store(&sState, RA_ERROR);
        return;
    }
    user = rc_client_get_user_info(client);
    if (user && user->username) {
        snprintf(sUserName, sizeof(sUserName), "%s", user->username);
        if (user->token && user->token[0]) Port_Config_SetRetroAchievementsSession(user->username, user->token);
    }
    atomic_store(&sState, RA_LOADING_GAME);
    if (sRomSupported) {
        rc_client_begin_identify_and_load_game(client, RC_CONSOLE_GAMEBOY_ADVANCE, "baserom.gba", gRomData,
                                               gRomSize, GameLoaded, NULL);
    } else {
        /* This port runs the USA decomp and exposes its GBA RAM layout. A
         * text-only PT-BR patch with the BZME base can therefore explicitly
         * opt into the USA set. Keep the translated ROM hash visible in the
         * diagnostic dump; the server request intentionally selects the USA
         * set only after this compatibility check. */
        rc_client_begin_load_game(client, kMinishCapUsaHash, GameLoaded, NULL);
    }
}
#endif

void Port_RetroAchievements_IdentifyLoadedRom(void) {
    sRomHash[0] = '\0';
    sRomSupported = 0;
    sUsaBaseCompatible = 0;
    if (gRomData == NULL || gRomSize == 0) return;

    if (!rc_hash_generate_from_buffer(sRomHash, RC_CONSOLE_GAMEBOY_ADVANCE, gRomData, gRomSize)) {
        sRomHash[0] = '\0';
        return;
    }
    sRomSupported = strcmp(sRomHash, kMinishCapUsaHash) == 0;
    sUsaBaseCompatible = gRomSize == 0x1000000u && memcmp(&gRomData[0xAC], "BZME", 4) == 0;
}

const char* Port_RetroAchievements_RomHash(void) {
    return sRomHash[0] ? sRomHash : NULL;
}

int Port_RetroAchievements_RomIsSupported(void) {
    return sRomSupported || sUsaBaseCompatible;
}

const char* Port_RetroAchievements_RomCompatibilityText(void) {
    if (sRomSupported) return "USA OFICIAL";
    if (sUsaBaseCompatible) return "PT-BR BASE USA";
    return "NAO SUPORTADA";
}

int Port_RetroAchievements_LoginInteractive(void) {
#ifdef TMC_3DS
    char username[64];
    char password[128];
    if (!Port_RetroAchievements_RomIsSupported()) {
        atomic_store(&sState, RA_UNSUPPORTED);
        return 0;
    }
    if (!Port_RetroAchievements3DS_BeginNetwork()) {
        atomic_store(&sState, RA_ERROR);
        return 0;
    }
    if (!sClient) {
        sClient = rc_client_create(ReadGbaMemory, ServerCall);
        if (sClient) rc_client_set_event_handler(sClient, RetroEvent);
    }
    if (!sClient) {
        atomic_store(&sState, RA_ERROR);
        return 0;
    }
    if (!Port_RetroAchievements3DS_PromptLogin(username, sizeof(username), password, sizeof(password))) {
        memset(password, 0, sizeof(password));
        return 0;
    }
    atomic_store(&sState, RA_SIGNING_IN);
    rc_client_begin_login_with_password(sClient, username, password, LoginFinished, NULL);
    memset(password, 0, sizeof(password));
    return atomic_load(&sState) == RA_ONLINE;
#else
    atomic_store(&sState, RA_UNSUPPORTED);
    return 0;
#endif
}

void Port_RetroAchievements_AutoLogin(void) {
#ifdef TMC_3DS
    char username[64];
    char token[128];
    if (!Port_RetroAchievements_RomIsSupported() ||
        !Port_Config_GetRetroAchievementsSession(username, sizeof(username), token, sizeof(token)))
        return;
    if (!Port_RetroAchievements3DS_BeginNetwork()) {
        atomic_store(&sState, RA_ERROR);
        return;
    }
    if (!sClient) {
        sClient = rc_client_create(ReadGbaMemory, ServerCall);
        if (sClient) rc_client_set_event_handler(sClient, RetroEvent);
    }
    if (!sClient) {
        atomic_store(&sState, RA_ERROR);
        return;
    }
    atomic_store(&sState, RA_SIGNING_IN);
    rc_client_begin_login_with_token(sClient, username, token, LoginFinished, NULL);
    memset(token, 0, sizeof(token));
#endif
}

void Port_RetroAchievements_Logout(void) {
    if (sClient) rc_client_logout(sClient);
    memset(sUserName, 0, sizeof(sUserName));
    memset(sAchievements, 0, sizeof(sAchievements));
    atomic_store(&sAchievementCount, 0);
    atomic_store(&sUnlockedCount, 0);
    sLastObservedUnlocked = 0;
#ifdef TMC_3DS
    Port_Config_ClearRetroAchievementsSession();
#endif
    atomic_store(&sState, RA_OFFLINE);
}

const char* Port_RetroAchievements_UserName(void) { return sUserName[0] ? sUserName : "NAO CONECTADA"; }

const char* Port_RetroAchievements_StatusText(void) {
    switch (atomic_load(&sState)) {
        case RA_SIGNING_IN: return "ENTRANDO";
        case RA_LOADING_GAME: return "CARREGANDO";
        case RA_ONLINE: return "CONECTADA";
        case RA_UNSUPPORTED: return "ROM NAO SUPORTADA";
        case RA_ERROR: return "ERRO DE REDE";
        default: return "NAO CONECTADA";
    }
}

void Port_RetroAchievements_Shutdown(void) {
#ifdef TMC_3DS
    /* A detached HTTPS request may still reference rcheevos callbacks. Keep
     * the client and libctru network service alive until it completes. */
    Port_RetroAchievements3DS_WaitForRequests();
#endif
    if (sClient) {
        rc_client_destroy(sClient);
        sClient = NULL;
    }
#ifdef TMC_3DS
    Port_RetroAchievements3DS_EndNetwork();
#endif
    memset(sUserName, 0, sizeof(sUserName));
    memset(sAchievements, 0, sizeof(sAchievements));
    atomic_store(&sAchievementCount, 0);
    atomic_store(&sUnlockedCount, 0);
    sLastObservedUnlocked = 0;
    atomic_store(&sState, RA_OFFLINE);
}

void Port_RetroAchievements_DoFrame(void) {
#ifdef TMC_3DS
    static unsigned pollFrames;
    if (sClient && atomic_load(&sState) == RA_ONLINE) {
        rc_client_do_frame(sClient);
        /* The upstream event is intentionally immediate, but an HTTPS
         * completion can race it on this small platform. Reconcile the
         * already-local official list once a second as a presentation-only
         * safety net: the server has already awarded the achievement, this
         * merely guarantees the player sees its confirmation. */
        if (++pollFrames >= 60) {
            const size_t before = sLastObservedUnlocked;
            pollFrames = 0;
            Port_RetroAchievements_RefreshList();
            if (atomic_load(&sUnlockedCount) > before) {
                const char* badge = NULL;
                const size_t count = atomic_load(&sAchievementCount);
                for (size_t i = 0; i < count; ++i) {
                    if (sAchievements[i].unlocked) badge = sAchievements[i].badgeName;
                }
                QueuePopup(PORT_RETRO_POPUP_ACHIEVEMENT_DETECTED, badge);
            }
            sLastObservedUnlocked = atomic_load(&sUnlockedCount);
        }
    }
#endif
}
