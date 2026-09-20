#ifndef TMC_PORT_RETROACHIEVEMENTS_H
#define TMC_PORT_RETROACHIEVEMENTS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Calculates the official RetroAchievements hash from the ROM already
 * resident in gRomData. This is deliberately local and read-only: it lets
 * the UI state the compatibility truth before any account or network code
 * is permitted to run. */
void Port_RetroAchievements_IdentifyLoadedRom(void);
const char* Port_RetroAchievements_RomHash(void);
int Port_RetroAchievements_RomIsSupported(void);
const char* Port_RetroAchievements_RomCompatibilityText(void);

/* Platform front ends own account entry and network transport. Credentials
 * are held only for the current process and are never persisted in saves or
 * settings. */
int Port_RetroAchievements_LoginInteractive(void);
void Port_RetroAchievements_AutoLogin(void);
void Port_RetroAchievements_Logout(void);
const char* Port_RetroAchievements_StatusText(void);
const char* Port_RetroAchievements_UserName(void);

/* A compact local copy of the official set. It is populated after the game
 * is loaded and lets the second screen present progress without additional
 * network requests while the player is browsing the menu. */
typedef struct {
    char title[48];
    char description[112];
    /* Stable RetroAchievements badge id. The 3DS front end caches its PNG
     * locally after login, so browsing and unlock popups never need a
     * network request. */
    char badgeName[32];
    uint32_t points;
    uint8_t unlocked;
} PortRetroAchievement;

/* A platform presentation can display these short, event-driven notices
 * without having to duplicate RetroAchievements' event handling. */
typedef enum {
    PORT_RETRO_POPUP_NONE = 0,
    PORT_RETRO_POPUP_ACHIEVEMENT_DETECTED,
    PORT_RETRO_POPUP_SUBMISSION_PENDING,
    PORT_RETRO_POPUP_SUBMISSION_COMPLETE,
} PortRetroPopup;

size_t Port_RetroAchievements_Count(void);
size_t Port_RetroAchievements_UnlockedCount(void);
int Port_RetroAchievements_Get(size_t index, PortRetroAchievement* out);
void Port_RetroAchievements_RefreshList(void);
uint32_t Port_RetroAchievements_PopupGeneration(void);
PortRetroPopup Port_RetroAchievements_PopupState(void);
int Port_RetroAchievements_PopupBadgeName(char* out, size_t outSize);
void Port_RetroAchievements_Shutdown(void);
void Port_RetroAchievements_DoFrame(void);

#ifdef __cplusplus
}
#endif

#endif
