#pragma once

/* The 3DS is neither POSIX nor Windows. Entropy is supplied by ps:ps in the
 * RetroAchievements transport instead of mbedTLS's platform poller. */
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_PLATFORM_MS_TIME_ALT
#undef MBEDTLS_TIMING_C
#undef MBEDTLS_NET_C
