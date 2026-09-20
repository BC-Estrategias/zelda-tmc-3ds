#ifndef PORT_ROOM_TRANSITION_PROFILE_H
#define PORT_ROOM_TRANSITION_PROFILE_H

#include <stdint.h>

/* Lightweight timing of the synchronous room-entry work.  It deliberately
 * observes only the existing loader; it never attempts to load a neighbour
 * or mutate game state.  The figures in a quick dump tell us which payload is
 * worth warming before we add a cache. */
typedef struct PortRoomTransitionProfile {
    uint64_t totalTicks;
    uint64_t gfxTicks;
    uint64_t roomInitTicks;
    uint64_t entityTicks;
    uint64_t mapVramTicks;
    uint32_t completedTransitions;
    uint8_t area;
    uint8_t room;
    uint8_t active;
} PortRoomTransitionProfile;

uint64_t Port_RoomTransitionProfile_Now(void);
void Port_RoomTransitionProfile_Begin(uint8_t area, uint8_t room);
void Port_RoomTransitionProfile_AddGfx(uint64_t ticks);
void Port_RoomTransitionProfile_AddRoomInit(uint64_t ticks);
void Port_RoomTransitionProfile_AddEntities(uint64_t ticks);
void Port_RoomTransitionProfile_AddMapVram(uint64_t ticks);
void Port_RoomTransitionProfile_End(void);
void Port_RoomTransitionProfile_Get(PortRoomTransitionProfile* out);

#endif
