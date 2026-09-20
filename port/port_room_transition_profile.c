#include "port_room_transition_profile.h"

#ifdef TMC_3DS
extern uint64_t Platform3DS_SystemTick(void);
#endif

static PortRoomTransitionProfile sProfile;
static uint64_t sStartTick;

uint64_t Port_RoomTransitionProfile_Now(void) {
#ifdef TMC_3DS
    return Platform3DS_SystemTick();
#else
    return 0;
#endif
}

void Port_RoomTransitionProfile_Begin(uint8_t area, uint8_t room) {
    sProfile.totalTicks = 0;
    sProfile.gfxTicks = 0;
    sProfile.roomInitTicks = 0;
    sProfile.entityTicks = 0;
    sProfile.mapVramTicks = 0;
    sProfile.area = area;
    sProfile.room = room;
    sProfile.active = 1;
    sStartTick = Port_RoomTransitionProfile_Now();
}

void Port_RoomTransitionProfile_AddGfx(uint64_t ticks) { if (sProfile.active) sProfile.gfxTicks += ticks; }
void Port_RoomTransitionProfile_AddRoomInit(uint64_t ticks) { if (sProfile.active) sProfile.roomInitTicks += ticks; }
void Port_RoomTransitionProfile_AddEntities(uint64_t ticks) { if (sProfile.active) sProfile.entityTicks += ticks; }
void Port_RoomTransitionProfile_AddMapVram(uint64_t ticks) { if (sProfile.active) sProfile.mapVramTicks += ticks; }

void Port_RoomTransitionProfile_End(void) {
    if (!sProfile.active) return;
    sProfile.totalTicks = Port_RoomTransitionProfile_Now() - sStartTick;
    sProfile.active = 0;
    ++sProfile.completedTransitions;
}

void Port_RoomTransitionProfile_Get(PortRoomTransitionProfile* out) {
    if (out != 0) *out = sProfile;
}
