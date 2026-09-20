#include "port_dump_state_3ds.h"

#include "fade.h"
#include "game.h"
#include "main.h"
#include "player.h"
#include "region.h"
#include "room.h"
#include "save.h"

#include "platform_3ds.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static bool CaptureCurrentState(SaveFile* captured) {
    if (captured == NULL || gMain.task != TASK_GAME || gMain.state != GAMETASK_MAIN) return false;

    memcpy(captured, &gSave, sizeof(*captured));
    PlayerRoomStatus* status = &captured->saved_status;
    memcpy(status, &gRoomTransition.player_status, sizeof(*status));
    status->area_next = gRoomControls.area;
    status->room_next = gRoomControls.room;
    status->start_anim = gPlayerEntity.base.animationState;
    status->spawn_type = (gPlayerState.flags & PL_MINISH) ? PL_SPAWN_MINISH : PL_SPAWN_DEFAULT;
    status->start_pos_x = gPlayerEntity.base.x.HALF.HI - gRoomControls.origin_x;
    status->start_pos_y = gPlayerEntity.base.y.HALF.HI - gRoomControls.origin_y;
    status->layer = gPlayerEntity.base.collisionLayer;
    return true;
}

bool Port_DumpState3DS_Write(const char* dumpDirectory) {
    char path[192];
    SaveFile captured;

    if (dumpDirectory == NULL ||
        snprintf(path, sizeof(path), "%s/%s", dumpDirectory, PORT_DUMP_LOAD_STATE_FILENAME) >= (int)sizeof(path)) {
        return false;
    }

    if (!CaptureCurrentState(&captured)) return false;
    return Port_DumpState_WriteFile(path, gActiveRegion, &captured);
}

bool Port_DumpState3DS_SaveQuick(void) {
    /* "quick" sorts after timestamped dump directories, so the existing
     * latest-state loader naturally picks this compact checkpoint first. */
    static const char quickDirectory[] = "dumps/dump-quick";
    if (mkdir("dumps", 0777) != 0 && errno != EEXIST) return false;
    if (mkdir(quickDirectory, 0777) != 0 && errno != EEXIST) return false;
    return Port_DumpState3DS_Write(quickDirectory);
}

bool Port_DumpState3DS_SaveSlot(unsigned slot) {
    char directory[48];
    if (slot == 0) return Port_DumpState3DS_SaveQuick();
    if (slot > 3) return false;
    if (mkdir("savestates", 0777) != 0 && errno != EEXIST) return false;
    if (snprintf(directory, sizeof(directory), "savestates/slot-%u", slot) >= (int)sizeof(directory)) return false;
    if (mkdir(directory, 0777) != 0 && errno != EEXIST) return false;
    return Port_DumpState3DS_Write(directory);
}

static PortDumpStateResult ApplyLoadedState(const PortDumpStateData* state) {
    if (state == NULL) return PORT_DUMP_STATE_INVALID;
    Platform3DS_MarkFrameDiscontinuity(OLD3DS_FRAME_PACER_DISCONTINUITY_DUMP);
    memcpy(&gSave, &state->save, sizeof(gSave));
    gSaveHeader->msg_speed = gSave.msg_speed;
    gSaveHeader->brightness = gSave.brightness;
    {
        extern void Rando_Runtime_Refresh(void);
        Rando_Runtime_Refresh();
    }
    SetFade(FADE_IN_OUT | FADE_INSTANT, 8);
    SetTask(TASK_GAME);
    return PORT_DUMP_STATE_OK;
}

PortDumpStateResult Port_DumpState3DS_LoadSlot(unsigned slot) {
    char path[80];
    PortDumpStateData state;
    PortDumpStateResult result;
    if (slot == 0) return Port_DumpState3DS_LoadLatest();
    if (slot > 3 || snprintf(path, sizeof(path), "savestates/slot-%u/%s", slot,
                              PORT_DUMP_LOAD_STATE_FILENAME) >= (int)sizeof(path))
        return PORT_DUMP_STATE_INVALID;
    result = Port_DumpState_ReadFile(path, gActiveRegion, &state);
    if (result != PORT_DUMP_STATE_OK) return result;
    return ApplyLoadedState(&state);
}

PortDumpStateResult Port_DumpState3DS_LoadLatest(void) {
    PortDumpStateData state;
    PortDumpStateResult result = Port_DumpState_ReadLatest("dumps", gActiveRegion, &state);

    if (result != PORT_DUMP_STATE_OK && result != PORT_DUMP_STATE_OK_LEGACY)
        return result;

    (void)ApplyLoadedState(&state);
    return result;
}
