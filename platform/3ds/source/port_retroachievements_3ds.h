#ifndef TMC_PORT_RETROACHIEVEMENTS_3DS_H
#define TMC_PORT_RETROACHIEVEMENTS_3DS_H

#include <stddef.h>

/* Kept separate from the game headers: libctru's legacy typedefs conflict
 * with the GBA decomp's typedefs when both are included in one translation
 * unit. */
int Port_RetroAchievements3DS_BeginNetwork(void);
int Port_RetroAchievements3DS_NetworkReady(void);
void Port_RetroAchievements3DS_EndNetwork(void);
int Port_RetroAchievements3DS_Request(const char* url, const char* postData, const char* contentType,
                                      char** body, size_t* bodyLength, int* httpStatus);
typedef void (*PortRetroAchievements3DS_ResponseCallback)(char* body, size_t bodyLength,
                                                           int httpStatus, int success, void* userdata);
int Port_RetroAchievements3DS_RequestAsync(const char* url, const char* postData, const char* contentType,
                                           PortRetroAchievements3DS_ResponseCallback callback, void* userdata);
void Port_RetroAchievements3DS_WaitForRequests(void);
/* Queues a low-priority, resumable local cache of official badge images.
 * Badge names are copied before this returns. */
void Port_RetroAchievements3DS_PrefetchBadges(const char* const* badgeNames, size_t count);
int Port_RetroAchievements3DS_PromptLogin(char* username, size_t usernameSize, char* password,
                                          size_t passwordSize);
const char* Port_RetroAchievements3DS_LastError(void);

#endif
