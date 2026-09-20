/* Included by the existing compositor: reuse Minish Cap's runtime-decoded
 * stone plate, red header chip, pale buttons and native message fonts. */
#include "updater.h"
#include <ctype.h>

static bool sUpdateConfirm, sUpdateNotes;
static unsigned sUpdatePage, sUpdatePages = 1;

static void UpdateUI_Reset(void) {
    UI_LOCK();
    sUpdateConfirm = sUpdateNotes = false;
    sUpdatePage = 0;
    UI_UNLOCK();
}

bool Port_SecondScreen_3DS_UpdateOpen(void) {
    UI_LOCK();
    bool open = sUi.tab == SS_TAB_SETTINGS && sUi.settingsPage == SS_SETTINGS_UPDATE;
    UI_UNLOCK();
    return open;
}

static void UpdateSmallText(const SSurf* s, const char* text, int x, int y, int style) {
    char label[128];
    snprintf(label,sizeof(label),"%s",text);
    for (char* c=label;*c;++c) *c=(char)toupper((unsigned char)*c);
    text=label;
    if (!Port_SecondScreenTheme_DrawBigText(s->px, s->w, s->h, s->stride, x, y, 1, style, text))
        DrawTextStr(s, text, x, y + 4, 1, Port_SecondScreenTheme_Color(SSC_MENU_INK), 0);
}

static int UpdateTextWidth(const char* text) {
    char label[128];
    snprintf(label,sizeof(label),"%s",text);
    for (char* c=label;*c;++c) *c=(char)toupper((unsigned char)*c);
    text=label;
    int w = Port_SecondScreenTheme_BigTextWidth(text, 1);
    return w ? w : TextWidthPx(text, 1);
}

static void UpdateCenteredText(const SSurf* s, const char* text, int cx, int y) {
    UpdateSmallText(s, text, cx - UpdateTextWidth(text)/2, y, SS_TEXT_NAVY);
}

static const char* UpdateLocalizedMessage(const UpdateStatus* st) {
    if (!st) return "";
    const char* m = st->message;
    if (!strcmp(m, "NO WI-FI CONNECTION")) return UiText3("SEM CONEXÃO WI-FI", "NO WI-FI CONNECTION", "SIN CONEXIÓN WI-FI");
    if (!strcmp(m, "INVALID RELEASE DATA")) return UiText3("DADOS DA VERSÃO INVÁLIDOS", "INVALID RELEASE DATA", "DATOS DE VERSIÓN INVÁLIDOS");
    if (!strcmp(m, "NO PRE-RELEASE AVAILABLE")) return UiText3("SEM PRÉ-VERSÃO DISPONÍVEL", "NO PRE-RELEASE AVAILABLE", "SIN PREVERSIÓN DISPONIBLE");
    if (!strcmp(m, "UPDATE AVAILABLE")) return UiText3("ATUALIZAÇÃO DISPONÍVEL", "UPDATE AVAILABLE", "ACTUALIZACIÓN DISPONIBLE");
    if (!strcmp(m, "YOU ARE UP TO DATE")) return UiText3("VOCÊ ESTÁ ATUALIZADO", "YOU ARE UP TO DATE", "ESTÁS ACTUALIZADO");
    if (!strcmp(m, "NOT ENOUGH SD SPACE")) return UiText3("ESPAÇO INSUFICIENTE NO SD", "NOT ENOUGH SD SPACE", "ESPACIO INSUFICIENTE EN SD");
    if (!strcmp(m, "DOWNLOADING UPDATE")) return UiText3("BAIXANDO ATUALIZAÇÃO", "DOWNLOADING UPDATE", "DESCARGANDO ACTUALIZACIÓN");
    if (!strcmp(m, "VERIFYING DOWNLOAD")) return UiText3("VERIFICANDO DOWNLOAD", "VERIFYING DOWNLOAD", "VERIFICANDO DESCARGA");
    if (!strcmp(m, "INSTALLING UPDATE")) return UiText3("INSTALANDO ATUALIZAÇÃO", "INSTALLING UPDATE", "INSTALANDO ACTUALIZACIÓN");
    if (!strcmp(m, "UPDATE INSTALLED")) return UiText3("ATUALIZAÇÃO INSTALADA", "UPDATE INSTALLED", "ACTUALIZACIÓN INSTALADA");
    if (!strcmp(m, "UPDATE CANCELLED")) return UiText3("ATUALIZAÇÃO CANCELADA", "UPDATE CANCELLED", "ACTUALIZACIÓN CANCELADA");
    if (!strcmp(m, "CONNECTION FAILED - RETRY")) return UiText3("CONEXÃO FALHOU - TENTE DE NOVO", "CONNECTION FAILED - RETRY", "CONEXIÓN FALLÓ - REINTENTA");
    if (!strcmp(m, "CHECKING FOR UPDATES")) return UiText3("BUSCANDO ATUALIZAÇÕES", "CHECKING FOR UPDATES", "BUSCANDO ACTUALIZACIONES");
    return m;
}

static void UpdateButton(const SSurf* s, TargetList* tl, float x0, float y0,
                          float x1, float y1, const char* text, int action, bool pressed,
                          float u, int ts) {
    DrawMenuButton(s, x0, y0, x1, y1, "", pressed, 0, u, ts);
    UpdateSmallText(s, text, (int)((x0+x1-UpdateTextWidth(text))/2),
                    (int)((y0+y1)/2-8), SS_TEXT_NAVY);
    if (action) AddTarget(tl, x0, y0, x1, y1, action, 0);
}

static void PaintUpdatePanel(const SSurf* s, TargetList* tl, float x0, float y0,
                              float x1, float y1, float u, int ts) {
    UpdateStatus st; Updater_GetStatus(&st);
    bool confirm, notes;
    UI_LOCK(); confirm = sUpdateConfirm; notes = sUpdateNotes; UI_UNLOCK();
    float row = 26, gap = 4;
    int cx = (int)((x0+x1)/2);
    char text[96];
    snprintf(text, sizeof(text), "%s: %s", UiText3("CANAL", "CHANNEL", "CANAL"),
             st.prerelease ? UiText3("PRÉ-VERSÃO", "PRE-RELEASE", "PREVERSIÓN")
                           : UiText3("ESTÁVEL", "STABLE", "ESTABLE"));
    UpdateButton(s, tl, x0,y0,x1,y0+row,text,SS_ACT_UPDATE_CHANNEL,false,u,ts);
    UpdateButton(s, tl, x0,y0+row+gap,x1,y0+2*row+gap,
                 st.version[0] ? st.version : UiText3("NENHUMA VERSÃO SELECIONADA", "NO RELEASE SELECTED", "NINGUNA VERSIÓN SELECCIONADA"),SS_ACT_UPDATE_RELEASE,notes,u,ts);
    char installed[64];
    snprintf(installed, sizeof(installed), "%s: V%s", UiText3("INSTALADA", "INSTALLED", "INSTALADA"), TMC_PORT_VERSION);
    UpdateCenteredText(s, installed, cx, (int)(y0+59));
    const char* message = confirm ? UiText3("BAIXAR E INSTALAR", "DOWNLOAD AND INSTALL", "DESCARGAR E INSTALAR")
                                  : UpdateLocalizedMessage(&st);
    if (!message[0]) message = UiText3("BUSCAR ATUALIZAÇÃO", "CHECK FOR UPDATE", "BUSCAR ACTUALIZACIÓN");
    // Long network errors are split at words inside the actual panel width.
    char first[80], second[80] = ""; snprintf(first,sizeof(first),"%s",message);
    if (UpdateTextWidth(first) > x1-x0) {
        char* split = strrchr(first,' ');
        while (split && UpdateTextWidth(first) > x1-x0) {
            *split = 0;
            snprintf(second,sizeof(second),"%s",message+strlen(first)+1);
            if (UpdateTextWidth(first) <= x1-x0) break;
            split = strrchr(first,' ');
        }
    }
    UpdateCenteredText(s,first,cx,(int)(y0+76));
    if (second[0]) UpdateCenteredText(s,second,cx,(int)(y0+91));
    else UpdateCenteredText(s, confirm ? UiText3("PROGRESSO NÃO SALVO SERÁ PERDIDO", "UNSAVED PROGRESS IS LOST", "SE PERDERÁ EL PROGRESO NO GUARDADO") :
                              notes ? UiText3("ALTERAÇÕES NA TELA SUPERIOR", "CHANGELOG ON TOP SCREEN", "CAMBIOS EN PANTALLA SUPERIOR")
                                    : UiText3("TOQUE NA VERSÃO PARA ALTERAÇÕES", "TAP RELEASE FOR CHANGELOG", "TOCA LA VERSIÓN PARA CAMBIOS"),
                              cx,(int)(y0+94));
    if (notes && !confirm && !Updater_Busy()) {
        UpdateButton(s,tl,x0,y0+112,x0+68,y0+133,UiText3("ANT.", "PREV", "ANT."),SS_ACT_UPDATE_PREV,false,u,ts);
        UpdateButton(s,tl,x1-68,y0+112,x1,y0+133,UiText3("PRÓX.", "NEXT", "SIG."),SS_ACT_UPDATE_NEXT,false,u,ts);
    }
    if (st.state == UPDATE_DOWNLOADING || st.state == UPDATE_VERIFYING || st.state == UPDATE_INSTALLING) {
        FillRect(s,(int)x0,(int)(y0+103),(int)x1,(int)(y0+111),Port_SecondScreenTheme_Color(SSC_MENU_STONE_DARK));
        FillRect(s,(int)x0,(int)(y0+103),(int)(x0+(x1-x0)*st.progress/100),(int)(y0+111),
                 Port_SecondScreenTheme_Color(SSC_RUPEE_GREEN));
    }
    const char* action = confirm ? UiText3("INSTALAR ATUALIZAÇÃO", "INSTALL UPDATE", "INSTALAR ACTUALIZACIÓN") :
        st.state == UPDATE_INSTALLING ? UiText3("AGUARDE", "PLEASE WAIT", "ESPERA") :
        Updater_Busy() ? UiText3("CANCELAR", "CANCEL", "CANCELAR") :
        st.state == UPDATE_AVAILABLE ? UiText3("BAIXAR ATUALIZAÇÃO", "DOWNLOAD UPDATE", "DESCARGAR ACTUALIZACIÓN") :
        st.state == UPDATE_DONE ? UiText3("ATUALIZAÇÃO INSTALADA", "UPDATE INSTALLED", "ACTUALIZACIÓN INSTALADA") :
        UiText3("BUSCAR ATUALIZAÇÃO", "CHECK FOR UPDATE", "BUSCAR ACTUALIZACIÓN");
    UpdateButton(s,tl,x0,y1-26,x1,y1,action,SS_ACT_UPDATE_ACTION,confirm,u,ts);
}

static bool HandleUpdateTap(int action, int arg) {
    if (!Port_SecondScreen_3DS_UpdateOpen()) return false;
    UpdateStatus st; Updater_GetStatus(&st);
    if (st.state == UPDATE_INSTALLING || st.state == UPDATE_DONE) return true;
    bool busy = Updater_Busy();
    if (action == SS_ACT_SETTINGS_BACK) {
        UI_LOCK(); bool confirm = sUpdateConfirm; sUpdateConfirm = false; UI_UNLOCK();
        if (confirm) return true;
        Updater_Cancel();
        if (busy) return true; // Wait for the worker before resuming the game.
        UpdateUI_Reset();
        return false;
    }
    if (action == SS_ACT_TAB) {
        if (busy) return true;
        UpdateUI_Reset(); return false;
    }
    if (action == SS_ACT_UPDATE_ACTION) {
        if (busy) { Updater_Cancel(); return true; }
        UI_LOCK(); bool confirm = sUpdateConfirm; sUpdateConfirm = false; UI_UNLOCK();
        if (confirm && st.state == UPDATE_AVAILABLE) Updater_Download();
        else if (st.state == UPDATE_AVAILABLE) {
            UI_LOCK(); sUpdateConfirm = true; UI_UNLOCK();
        } else Updater_Check();
        return true;
    }
    if (busy) return true;
    switch (action) {
        case SS_ACT_UPDATE_CHANNEL:
            UpdateUI_Reset(); Updater_SetChannel(!st.prerelease); break;
        case SS_ACT_UPDATE_RELEASE:
            if (st.version[0]) { UI_LOCK(); sUpdateNotes = true; sUpdatePage = 0; UI_UNLOCK(); }
            break;
        case SS_ACT_UPDATE_PREV:
            UI_LOCK(); if (sUpdatePage) --sUpdatePage; UI_UNLOCK(); break;
        case SS_ACT_UPDATE_NEXT:
            UI_LOCK(); if (sUpdatePage+1 < sUpdatePages) ++sUpdatePage; UI_UNLOCK(); break;
        default: return true;
    }
    return true;
}

/* Main-thread top painter. Theme caches have been initialized before either
 * screen worker starts; the top view reads only their immutable art. */
bool Port_SecondScreen_3DS_PaintUpdateTop(uint32_t* pixels, int stride) {
    if (!Port_SecondScreen_3DS_UpdateOpen()) return false;
    UpdateStatus st; Updater_GetStatus(&st);
    bool notes; unsigned page;
    UI_LOCK(); notes = sUpdateNotes; page = sUpdatePage; UI_UNLOCK();
    static unsigned paintedRevision = UINT32_MAX, paintedPage = UINT32_MAX;
    static bool paintedNotes;
    if (paintedRevision == st.revision && paintedPage == page && paintedNotes == notes) return false;
    paintedRevision=st.revision; paintedPage=page; paintedNotes=notes;
    static char lines[512][UPDATE_NOTE_LINE_BYTES];
    static unsigned count, revision = UINT32_MAX;
    static bool lastNotes;
    if (revision != st.revision || notes != lastNotes) {
        static char body[12289], formatted[512][UPDATE_NOTE_LINE_BYTES];
        if (notes && st.version[0]) Updater_GetNotes(body,sizeof(body));
        else snprintf(body,sizeof(body),"%s: v%s\n\n%s\n\n%s\n%s",
            UiText3("Instalada", "Installed", "Instalada"), TMC_PORT_VERSION, UpdateLocalizedMessage(&st),
            UiText3("Selecione uma versão abaixo para ler as alterações.", "Select a release below to read its changelog.", "Selecciona una versión abajo para leer los cambios."),
            UiText3("Escolha Estável ou Pré-versão para verificar o canal.", "Choose Stable or Pre-release to check that channel.", "Elige Estable o Preversión para comprobar el canal."));
        unsigned n = Update_FormatNotes(body,formatted,512);
        count = 0;
        for (unsigned i=0; i<n && count<512; i++)
            snprintf(lines[count++], UPDATE_NOTE_LINE_BYTES, "%s", formatted[i]);
        revision=st.revision; lastNotes=notes;
    }
    unsigned pages = (count+8)/9; if (!pages) pages=1;
    if (page>=pages) page=pages-1;
    UI_LOCK(); sUpdatePages=pages; sUpdatePage=page; UI_UNLOCK();
    SSurf s={pixels,400,240,stride};
    Port_SecondScreenTheme_DrawBackdrop(pixels,400,240,stride,0,0,400,240,2);
    Port_SecondScreenTheme_DrawPlate(pixels,400,240,stride,4,4,392,232,2);
    char title[80]; snprintf(title,sizeof(title),notes ? "%s %s" : "%s%s",
        notes ? UiText3("ALTERAÇÕES", "CHANGELOG", "CAMBIOS") : "",
        notes ? st.version : "",
        notes ? "" : UiText3("ATUALIZAÇÃO", "UPDATE", "ACTUALIZACIÓN"),
        "");
    for(char* c=title;*c;++c) *c=(char)toupper((unsigned char)*c);
    DrawPanelHeaderChip(&s,200,14,title,1,1.0f/3);
    Port_SecondScreenTheme_DrawWell(pixels,400,240,stride,17,47,366,165,1);
    for (unsigned i=page*9;i<count && i<(page+1)*9;i++)
        UpdateSmallText(&s,lines[i],23,49+(i-page*9)*18,SS_TEXT_INK);
    char footer[64]; snprintf(footer,sizeof(footer),"%s %u %s %u",
        UiText3("PÁGINA", "PAGE", "PÁGINA"), page+1, UiText3("DE", "OF", "DE"), pages);
    const int footerW=UpdateTextWidth(footer)+24;
    const int footerY=211, footerH=24;
    DrawMenuButton(&s,200-footerW/2,footerY,200+(footerW+1)/2,footerY+footerH,"",0,0,1.f/3,1);
    // The Settings banner font has visible ink on rows 2..14. Center that
    // thirteen-pixel band, rather than the full sixteen-pixel glyph cell.
    UpdateSmallText(&s,footer,200-UpdateTextWidth(footer)/2,
                    footerY+(footerH-13)/2-2,SS_TEXT_NAVY);
    return true;
}
