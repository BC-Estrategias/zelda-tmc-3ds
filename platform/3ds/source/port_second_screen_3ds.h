#ifndef TMC_PORT_SECOND_SCREEN_3DS_H
#define TMC_PORT_SECOND_SCREEN_3DS_H

#include <stdint.h>
#include <stdbool.h>

#include "bottom_frame_state_3ds.h"
#include "port_second_screen_state.h"

#ifdef __cplusplus
extern "C" {
#endif

bool Port_SecondScreen_3DS_UpdateOpen(void);
bool Port_SecondScreen_3DS_PaintUpdateTop(uint32_t* pixels, int stride);

uint32_t Port_SecondScreen_3DS_PaintInto(uint32_t* pixels, int width, int height, int strideInPixels,
                                        const SecondScreenSnapshot* snap, uint32_t tick);
void Port_SecondScreen_3DS_ResetFrameState(void);
uint32_t Port_SecondScreen_3DS_RequestRefresh(void);
void Port_SecondScreen_3DS_MarkSubmitted(uint32_t generation, int inGame);
void Port_SecondScreen_3DS_PromoteSubmitted(void);
void Port_SecondScreen_3DS_GetFrameStats(BottomFrameState3DSStats* out);
void Port_SecondScreen_3DS_OnTap(int x, int y, int longPress);
/* Controller shortcut: advance Map -> Items -> Missions -> Map. */
void Port_SecondScreen_3DS_CycleTab(void);
/* Opens the destructive load-state confirmation overlay. */
void Port_SecondScreen_3DS_RequestLoadState(void);
int Port_SecondScreen_3DS_LoadConfirmationActive(void);
void Port_SecondScreen_3DS_ConfirmLoadState(void);
void Port_SecondScreen_3DS_CancelLoadState(void);
int Port_SecondScreen_3DS_NeedsRefresh(void);
int Port_SecondScreen_3DS_NeedsPeriodicRefresh(const SecondScreenSnapshot* snap);
int Port_SecondScreen_3DS_SnapshotChangeNeedsRefresh(const SecondScreenSnapshot* previous,
                                                     const SecondScreenSnapshot* current,
                                                     int previousValid);

#ifdef __cplusplus
}
#endif

#endif
