#ifndef PORT_DUMP_STATE_3DS_H
#define PORT_DUMP_STATE_3DS_H

#include <stdbool.h>

#include "port_dump_state.h"

bool Port_DumpState3DS_Write(const char* dumpDirectory);
/* Fast savestate for controller shortcuts: only the validated load-state
 * record, never the screenshots and diagnostic memory blobs of a full dump. */
bool Port_DumpState3DS_SaveQuick(void);
/* Slot 0 is the quick state; slots 1 through 3 are named persistent states. */
bool Port_DumpState3DS_SaveSlot(unsigned slot);
PortDumpStateResult Port_DumpState3DS_LoadLatest(void);
PortDumpStateResult Port_DumpState3DS_LoadSlot(unsigned slot);

#endif // PORT_DUMP_STATE_3DS_H
