/*
 * The upstream auto-updater uses devkitPro's optional curl port.  It is not
 * part of the reproducible build toolchain used for this edition, whose
 * releases are installed through the GitHub QR code instead. Keep the UI
 * entry points inert so the rest of the upstream runtime remains compatible.
 */
#include "updater.h"

void Updater_Init(const char* launch_path) { (void)launch_path; }
void Updater_Check(void) {}
void Updater_SetChannel(bool prerelease) { (void)prerelease; }
void Updater_Download(void) {}
void Updater_Cancel(void) {}
void Updater_GetStatus(UpdateStatus* out) {
    if (!out) return;
    out->state = UPDATE_IDLE;
    out->prerelease = false;
    out->progress = 0;
    out->revision = 0;
    out->version[0] = '\0';
    out->message[0] = '\0';
}
bool Updater_Busy(void) { return false; }
bool Updater_ShouldClose(void) { return false; }
void Updater_Shutdown(void) {}
unsigned Updater_GetNotes(char* out, unsigned capacity) {
    if (out && capacity) out[0] = '\0';
    return 0;
}

bool Port_SecondScreen_3DS_UpdateOpen(void) { return false; }
