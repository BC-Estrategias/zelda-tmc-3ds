#ifndef PORT_BUTTON_MAPPING_H
#define PORT_BUTTON_MAPPING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PORT_3DS_MAP_X = 0,
    PORT_3DS_MAP_Y,
    PORT_3DS_MAP_ZL,
    PORT_3DS_MAP_ZR,
    PORT_3DS_MAP_CSTICK,
    PORT_3DS_MAP_COUNT
} Port3DSMapButton;

typedef enum {
    PORT_3DS_ACTION_NONE = 0,
    PORT_3DS_ACTION_TABS,
    PORT_3DS_ACTION_TURBO,
    PORT_3DS_ACTION_ITEM,
    PORT_3DS_ACTION_SAVE_STATE,
    PORT_3DS_ACTION_LOAD_STATE,
    PORT_3DS_ACTION_COUNT
} Port3DSMappedAction;

int Port_Config_Get3DSButtonAction(int button);
void Port_Config_Set3DSButtonAction(int button, int action);
void Port_Config_Cycle3DSButtonAction(int button);
const char* Port_Config_Get3DSButtonActionName(int action);

#ifdef __cplusplus
}
#endif

#endif
