#include "port_retroachievements_3ds.h"

#include <3ds.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/platform_time.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <psa/crypto.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#include "stb_image.h"

enum { SOC_BUFFER_SIZE = 0x100000, RESPONSE_LIMIT = 524288 };
static int sReady;
static u32* sSocBuffer;
static char sLastError[24] = "SEM ERRO";
static _Atomic unsigned int sAsyncRequests;
static _Atomic int sBadgePrefetchActive;

typedef struct {
    char* url;
    char* post;
    char* contentType;
    PortRetroAchievements3DS_ResponseCallback callback;
    void* userdata;
} AsyncRequest;

/* Google Trust Services GTS Root R4, self-signed DER. */
static const u8 kRootCa[] = {
0x30,0x82,0x02,0x09,0x30,0x82,0x01,0x8e,0xa0,0x03,0x02,0x01,0x02,0x02,0x0d,0x02,0x03,0xe5,0xc0,0x68,0xef,0x63,0x1a,0x9c,0x72,0x90,0x50,0x52,0x30,0x0a,0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x03,0x30,0x47,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x22,0x30,0x20,0x06,0x03,0x55,0x04,0x0a,0x13,0x19,0x47,0x6f,0x6f,0x67,0x6c,0x65,0x20,0x54,0x72,0x75,0x73,0x74,0x20,0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x73,0x20,0x4c,0x4c,0x43,0x31,0x14,0x30,0x12,0x06,0x03,0x55,0x04,0x03,0x13,0x0b,0x47,0x54,0x53,0x20,0x52,0x6f,0x6f,0x74,0x20,0x52,0x34,0x30,0x1e,0x17,0x0d,0x31,0x36,0x30,0x36,0x32,0x32,0x30,0x30,0x30,0x30,0x30,0x30,0x5a,0x17,0x0d,0x33,0x36,0x30,0x36,0x32,0x32,0x30,0x30,0x30,0x30,0x30,0x30,0x5a,0x30,0x47,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x22,0x30,0x20,0x06,0x03,0x55,0x04,0x0a,0x13,0x19,0x47,0x6f,0x6f,0x67,0x6c,0x65,0x20,0x54,0x72,0x75,0x73,0x74,0x20,0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x73,0x20,0x4c,0x4c,0x43,0x31,0x14,0x30,0x12,0x06,0x03,0x55,0x04,0x03,0x13,0x0b,0x47,0x54,0x53,0x20,0x52,0x6f,0x6f,0x74,0x20,0x52,0x34,0x30,0x76,0x30,0x10,0x06,0x07,0x2a,0x86,0x48,0xce,0x3d,0x02,0x01,0x06,0x05,0x2b,0x81,0x04,0x00,0x22,0x03,0x62,0x00,0x04,0xf3,0x74,0x73,0xa7,0x68,0x8b,0x60,0xae,0x43,0xb8,0x35,0xc5,0x81,0x30,0x7b,0x4b,0x49,0x9d,0xfb,0xc1,0x61,0xce,0xe6,0xde,0x46,0xbd,0x6b,0xd5,0x61,0x18,0x35,0xae,0x40,0xdd,0x73,0xf7,0x89,0x91,0x30,0x5a,0xeb,0x3c,0xee,0x85,0x7c,0xa2,0x40,0x76,0x3b,0xa9,0xc6,0xb8,0x47,0xd8,0x2a,0xe7,0x92,0x91,0x6a,0x73,0xe9,0xb1,0x72,0x39,0x9f,0x29,0x9f,0xa2,0x98,0xd3,0x5f,0x5e,0x58,0x86,0x65,0x0f,0xa1,0x84,0x65,0x06,0xd1,0xdc,0x8b,0xc9,0xc7,0x73,0xc8,0x8c,0x6a,0x2f,0xe5,0xc4,0xab,0xd1,0x1d,0x8a,0xa3,0x42,0x30,0x40,0x30,0x0e,0x06,0x03,0x55,0x1d,0x0f,0x01,0x01,0xff,0x04,0x04,0x03,0x02,0x01,0x86,0x30,0x0f,0x06,0x03,0x55,0x1d,0x13,0x01,0x01,0xff,0x04,0x05,0x30,0x03,0x01,0x01,0xff,0x30,0x1d,0x06,0x03,0x55,0x1d,0x0e,0x04,0x16,0x04,0x14,0x80,0x4c,0xd6,0xeb,0x74,0xff,0x49,0x36,0xa3,0xd5,0xd8,0xfc,0xb5,0x3e,0xc5,0x6a,0xf0,0x94,0x1d,0x8c,0x30,0x0a,0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x03,0x03,0x69,0x00,0x30,0x66,0x02,0x31,0x00,0xe8,0x40,0xff,0x83,0xde,0x03,0xf4,0x9f,0xae,0x1d,0x7a,0xa7,0x2e,0xb9,0xaf,0x4f,0xf6,0x83,0x1d,0x0e,0x2d,0x85,0x01,0x1d,0xd1,0xd9,0x6a,0xec,0x0f,0xc2,0xaf,0xc7,0x5e,0x56,0x5e,0x5c,0xd5,0x1c,0x58,0x22,0x28,0x0b,0xf7,0x30,0xb6,0x2f,0xb1,0x7c,0x02,0x31,0x00,0xf0,0x61,0x3c,0xa7,0xf4,0xa0,0x82,0xe3,0x21,0xd5,0x84,0x1d,0x73,0x86,0x9c,0x2d,0xaf,0xca,0x34,0x9b,0xf1,0x9f,0xb9,0x23,0x36,0xe2,0xbc,0x60,0x03,0x9d,0x80,0xb3,0x9a,0x56,0xc8,0xe1,0xe2,0xbb,0x14,0x79,0xca,0xcd,0x21,0xd4,0x94,0xb5,0x49,0x43};

mbedtls_ms_time_t mbedtls_ms_time(void) { return (mbedtls_ms_time_t)osGetTime(); }
int mbedtls_hardware_poll(void* p, unsigned char* out, size_t len, size_t* olen) { (void)p; if (R_FAILED(PS_GenerateRandomBytes(out, len))) return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED; *olen = len; return 0; }
static void Error(const char* stage, int code) { snprintf(sLastError, sizeof(sLastError), "%s %04X", stage, (unsigned)(-code)); }
const char* Port_RetroAchievements3DS_LastError(void) { return sLastError; }

static int SendCb(void* p, const unsigned char* b, size_t n) { int r = send(*(int*)p, b, n, 0); return r >= 0 ? r : (errno == EAGAIN ? MBEDTLS_ERR_SSL_WANT_WRITE : MBEDTLS_ERR_NET_SEND_FAILED); }
static int RecvCb(void* p, unsigned char* b, size_t n) { int r = recv(*(int*)p, b, n, 0); return r > 0 ? r : (r == 0 ? MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY : (errno == EAGAIN ? MBEDTLS_ERR_SSL_WANT_READ : MBEDTLS_ERR_NET_RECV_FAILED)); }

static int DecodeChunked(char* data, size_t* length) {
    char *source = data, *target = data, *lineEnd;
    unsigned long chunk;
    while (1) {
        lineEnd = strstr(source, "\r\n");
        if (!lineEnd) return 0;
        *lineEnd = '\0';
        chunk = strtoul(source, NULL, 16);
        source = lineEnd + 2;
        if (chunk == 0) { *target = '\0'; *length = (size_t)(target - data); return 1; }
        if (chunk > *length || source + chunk + 2 > data + *length) return 0;
        memmove(target, source, chunk);
        target += chunk;
        source += chunk;
        if (source[0] != '\r' || source[1] != '\n') return 0;
        source += 2;
    }
}

int Port_RetroAchievements3DS_BeginNetwork(void) {
    Result r;
    if (sReady) return 1;
    sSocBuffer = memalign(0x1000, SOC_BUFFER_SIZE);
    if (!sSocBuffer) { snprintf(sLastError, sizeof(sLastError), "SEM MEMORIA"); return 0; }
    if (R_FAILED(r = socInit(sSocBuffer, SOC_BUFFER_SIZE))) { Error("SOC", r); free(sSocBuffer); sSocBuffer = NULL; return 0; }
    if (R_FAILED(r = psInit())) { Error("RNG", r); socExit(); free(sSocBuffer); sSocBuffer = NULL; return 0; }
    if (psa_crypto_init() != PSA_SUCCESS) { snprintf(sLastError, sizeof(sLastError), "CRYPTO"); psExit(); socExit(); free(sSocBuffer); sSocBuffer = NULL; return 0; }
    sReady = 1; snprintf(sLastError, sizeof(sLastError), "SEM ERRO"); return 1;
}
void Port_RetroAchievements3DS_EndNetwork(void) { if (sReady) { psExit(); socExit(); } free(sSocBuffer); sSocBuffer = NULL; sReady = 0; }

int Port_RetroAchievements3DS_Request(const char* url, const char* post, const char* type, char** body, size_t* bodyLen, int* status) {
    const char *p, *slash; char host[96], request[2048], *raw = NULL, *data; size_t hlen, reqLen, total = 0, sent = 0;
    int fd = -1, r = -1, got, chunked; struct hostent* he; struct sockaddr_in addr;
    mbedtls_entropy_context entropy; mbedtls_ctr_drbg_context drbg; mbedtls_x509_crt ca; mbedtls_ssl_config conf; mbedtls_ssl_context ssl;
    if (!sReady || !url || !body || !bodyLen || !status || strncmp(url, "https://", 8)) return 0;
    *body = NULL; *bodyLen = 0; *status = -1; p = url + 8; slash = strchr(p, '/'); hlen = slash ? (size_t)(slash-p) : strlen(p);
    if (!hlen || hlen >= sizeof(host)) return 0; memcpy(host,p,hlen); host[hlen]=0;
    mbedtls_entropy_init(&entropy); mbedtls_ctr_drbg_init(&drbg); mbedtls_x509_crt_init(&ca); mbedtls_ssl_config_init(&conf); mbedtls_ssl_init(&ssl);
    if ((r=mbedtls_ctr_drbg_seed(&drbg,mbedtls_entropy_func,&entropy,(const u8*)"tmc3ds-ra",8))) { Error("RNG",r); goto done; }
    if ((r=mbedtls_x509_crt_parse_der(&ca,kRootCa,sizeof(kRootCa)))) { Error("CA",r); goto done; }
    he=gethostbyname(host); if (!he || !he->h_addr_list[0]) { snprintf(sLastError,sizeof(sLastError),"DNS"); goto done; }
    memset(&addr,0,sizeof(addr)); addr.sin_family=AF_INET; addr.sin_port=htons(443); memcpy(&addr.sin_addr,he->h_addr_list[0],4);
    if ((fd=socket(AF_INET,SOCK_STREAM,0))<0 || connect(fd,(struct sockaddr*)&addr,sizeof(addr))) { snprintf(sLastError,sizeof(sLastError),"CONECTAR"); goto done; }
    if ((r=mbedtls_ssl_config_defaults(&conf,MBEDTLS_SSL_IS_CLIENT,MBEDTLS_SSL_TRANSPORT_STREAM,MBEDTLS_SSL_PRESET_DEFAULT))) { Error("TLS",r); goto done; }
    mbedtls_ssl_conf_authmode(&conf,MBEDTLS_SSL_VERIFY_REQUIRED); mbedtls_ssl_conf_ca_chain(&conf,&ca,NULL); mbedtls_ssl_conf_rng(&conf,mbedtls_ctr_drbg_random,&drbg);
    if ((r=mbedtls_ssl_setup(&ssl,&conf)) || (r=mbedtls_ssl_set_hostname(&ssl,host))) { Error("TLS",r); goto done; }
    mbedtls_ssl_set_bio(&ssl,&fd,SendCb,RecvCb,NULL); do r=mbedtls_ssl_handshake(&ssl); while(r==MBEDTLS_ERR_SSL_WANT_READ||r==MBEDTLS_ERR_SSL_WANT_WRITE);
    if (r) { Error("TLS",r); goto done; } if (mbedtls_ssl_get_verify_result(&ssl)) { snprintf(sLastError,sizeof(sLastError),"CERTIFICADO"); goto done; }
    reqLen=(size_t)snprintf(request,sizeof(request),"%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: TheMinishCap3DS/1.3\r\nAccept: application/json\r\nConnection: close\r\n%s%s%sContent-Length: %u\r\n\r\n%s",post&&*post?"POST":"GET",slash?slash:"/",host,post&&*post&&type?"Content-Type: ":"",post&&*post&&type?type:"",post&&*post&&type?"\r\n":"",(unsigned)(post&&*post?strlen(post):0),post&&*post?post:"");
    if(reqLen>=sizeof(request)){snprintf(sLastError,sizeof(sLastError),"PEDIDO GRANDE");goto done;} while(sent<reqLen){r=mbedtls_ssl_write(&ssl,(const u8*)request+sent,reqLen-sent);if(r>0)sent+=(size_t)r;else if(r!=MBEDTLS_ERR_SSL_WANT_READ&&r!=MBEDTLS_ERR_SSL_WANT_WRITE){Error("ENVIAR",r);goto done;}}
    if(!(raw=malloc(RESPONSE_LIMIT+1))){snprintf(sLastError,sizeof(sLastError),"SEM MEMORIA");goto done;} while(total<RESPONSE_LIMIT){got=mbedtls_ssl_read(&ssl,(u8*)raw+total,RESPONSE_LIMIT-total);if(got>0){total+=got;continue;}if(got==0||got==MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)break;if(got!=MBEDTLS_ERR_SSL_WANT_READ&&got!=MBEDTLS_ERR_SSL_WANT_WRITE){Error("DADOS",got);goto done;}}
    raw[total]=0; if(sscanf(raw,"HTTP/%*s %d",status)!=1 || !(data=strstr(raw,"\r\n\r\n"))){snprintf(sLastError,sizeof(sLastError),"RESPOSTA");goto done;} chunked=strstr(raw,"Transfer-Encoding: chunked")!=NULL||strstr(raw,"transfer-encoding: chunked")!=NULL; data+=4; *bodyLen=total-(size_t)(data-raw); memmove(raw,data,*bodyLen);raw[*bodyLen]=0;if(chunked&&!DecodeChunked(raw,bodyLen)){snprintf(sLastError,sizeof(sLastError),"BLOCOS");goto done;}*body=raw;raw=NULL;r=0;
done: if(fd>=0)closesocket(fd);free(raw);mbedtls_ssl_free(&ssl);mbedtls_ssl_config_free(&conf);mbedtls_x509_crt_free(&ca);mbedtls_ctr_drbg_free(&drbg);mbedtls_entropy_free(&entropy);return r==0;
}

static char* CopyRequestText(const char* text) {
    size_t length = text ? strlen(text) + 1 : 1;
    char* copy = malloc(length);
    if (copy) memcpy(copy, text ? text : "", length);
    return copy;
}

static void AsyncRequestWorker(void* userdata) {
    AsyncRequest* request = (AsyncRequest*)userdata;
    char* body = NULL;
    size_t bodyLength = 0;
    int status = -1;
    int success = request && Port_RetroAchievements3DS_Request(request->url, request->post,
                                                                 request->contentType, &body,
                                                                 &bodyLength, &status);
    if (request->callback) request->callback(body, bodyLength, status, success, request->userdata);
    free(body);
    free(request->url);
    free(request->post);
    free(request->contentType);
    free(request);
    atomic_fetch_sub(&sAsyncRequests, 1);
}

int Port_RetroAchievements3DS_RequestAsync(const char* url, const char* post, const char* type,
                                           PortRetroAchievements3DS_ResponseCallback callback, void* userdata) {
    AsyncRequest* request;
    Thread thread;
    if (!url || !callback) return 0;
    request = calloc(1, sizeof(*request));
    if (!request) return 0;
    request->url = CopyRequestText(url);
    request->post = CopyRequestText(post);
    request->contentType = CopyRequestText(type);
    request->callback = callback;
    request->userdata = userdata;
    if (!request->url || !request->post || !request->contentType) {
        free(request->url); free(request->post); free(request->contentType); free(request);
        return 0;
    }
    atomic_fetch_add(&sAsyncRequests, 1);
    thread = threadCreate(AsyncRequestWorker, request, 32 * 1024, 0x30, -1, true);
    if (!thread) {
        atomic_fetch_sub(&sAsyncRequests, 1);
        free(request->url); free(request->post); free(request->contentType); free(request);
        return 0;
    }
    threadDetach(thread);
    return 1;
}

void Port_RetroAchievements3DS_WaitForRequests(void) {
    while (atomic_load(&sAsyncRequests) != 0) svcSleepThread(1000000LL);
}

enum { BADGE_CACHE_LIMIT = 96, BADGE_NAME_SIZE = 32, BADGE_RGBA_BYTES = 64 * 64 * 4 };
typedef struct {
    size_t count;
    char names[BADGE_CACHE_LIMIT][BADGE_NAME_SIZE];
} BadgePrefetch;

static int BadgeNameIsSafe(const char* name) {
    if (!name || !name[0]) return 0;
    for (; *name; ++name) {
        if (!((*name >= 'a' && *name <= 'z') || (*name >= 'A' && *name <= 'Z') ||
              (*name >= '0' && *name <= '9') || *name == '_' || *name == '-')) return 0;
    }
    return 1;
}

static int BadgeAlreadyCached(const char* name) {
    char path[96];
    struct stat st;
    snprintf(path, sizeof(path), "ra_badges/%s.rgba", name);
    return stat(path, &st) == 0 && st.st_size == BADGE_RGBA_BYTES;
}

static void CacheBadge(const char* name) {
    char url[192], path[96], temporary[104];
    char* png = NULL;
    size_t pngLength = 0;
    int status = 0, width = 0, height = 0, components = 0;
    stbi_uc* rgba;
    FILE* file;
    if (!BadgeNameIsSafe(name) || BadgeAlreadyCached(name)) return;
    snprintf(url, sizeof(url), "https://media.retroachievements.org/Badge/%s.png", name);
    if (!Port_RetroAchievements3DS_Request(url, "", "", &png, &pngLength, &status) ||
        status != 200 || !png || pngLength == 0) {
        free(png);
        return;
    }
    rgba = stbi_load_from_memory((const stbi_uc*)png, (int)pngLength, &width, &height, &components, 4);
    free(png);
    if (!rgba || width != 64 || height != 64) {
        stbi_image_free(rgba);
        return;
    }
    snprintf(path, sizeof(path), "ra_badges/%s.rgba", name);
    snprintf(temporary, sizeof(temporary), "%s.tmp", path);
    file = fopen(temporary, "wb");
    if (file) {
        if (fwrite(rgba, 1, BADGE_RGBA_BYTES, file) == BADGE_RGBA_BYTES) {
            fclose(file);
            rename(temporary, path);
        } else {
            fclose(file);
            remove(temporary);
        }
    }
    stbi_image_free(rgba);
}

static void BadgePrefetchWorker(void* userdata) {
    BadgePrefetch* job = (BadgePrefetch*)userdata;
    mkdir("ra_badges", 0777);
    if (job) {
        for (size_t i = 0; i < job->count; ++i) {
            CacheBadge(job->names[i]);
            /* A badge cache must never compete with gameplay or the normal
             * RetroAchievements protocol. One quiet request at a time. */
            svcSleepThread(100000000LL);
        }
    }
    free(job);
    atomic_store(&sBadgePrefetchActive, 0);
    atomic_fetch_sub(&sAsyncRequests, 1);
}

void Port_RetroAchievements3DS_PrefetchBadges(const char* const* badgeNames, size_t count) {
    BadgePrefetch* job;
    Thread thread;
    if (!badgeNames || count == 0 || !sReady || atomic_exchange(&sBadgePrefetchActive, 1)) return;
    if (count > BADGE_CACHE_LIMIT) count = BADGE_CACHE_LIMIT;
    job = calloc(1, sizeof(*job));
    if (!job) { atomic_store(&sBadgePrefetchActive, 0); return; }
    for (size_t i = 0; i < count; ++i) {
        if (BadgeNameIsSafe(badgeNames[i])) {
            snprintf(job->names[job->count], BADGE_NAME_SIZE, "%s", badgeNames[i]);
            ++job->count;
        }
    }
    if (job->count == 0) { free(job); atomic_store(&sBadgePrefetchActive, 0); return; }
    atomic_fetch_add(&sAsyncRequests, 1);
    thread = threadCreate(BadgePrefetchWorker, job, 64 * 1024, 0x3A, -1, true);
    if (!thread) {
        atomic_fetch_sub(&sAsyncRequests, 1);
        free(job);
        atomic_store(&sBadgePrefetchActive, 0);
        return;
    }
    threadDetach(thread);
}

int Port_RetroAchievements3DS_PromptLogin(char* user,size_t us,char* pass,size_t ps){SwkbdState k;if(!user||!pass||us<2||ps<2)return 0;swkbdInit(&k,SWKBD_TYPE_QWERTY,2,(int)us-1);swkbdSetButton(&k,SWKBD_BUTTON_LEFT,"CANCELAR",false);swkbdSetButton(&k,SWKBD_BUTTON_RIGHT,"OK",true);swkbdSetHintText(&k,"USUARIO RETROACHIEVEMENTS");swkbdSetValidation(&k,SWKBD_NOTEMPTY_NOTBLANK,0,0);user[0]=0;if(swkbdInputText(&k,user,us)!=SWKBD_BUTTON_CONFIRM||!user[0])return 0;swkbdInit(&k,SWKBD_TYPE_QWERTY,2,(int)ps-1);swkbdSetButton(&k,SWKBD_BUTTON_LEFT,"CANCELAR",false);swkbdSetButton(&k,SWKBD_BUTTON_RIGHT,"OK",true);swkbdSetHintText(&k,"SENHA (NAO SERA SALVA)");swkbdSetValidation(&k,SWKBD_NOTEMPTY_NOTBLANK,0,0);swkbdSetPasswordMode(&k,SWKBD_PASSWORD_HIDE);pass[0]=0;return swkbdInputText(&k,pass,ps)==SWKBD_BUTTON_CONFIRM&&pass[0];}
