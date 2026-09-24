#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_BOOT = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *boot;
    lv_obj_t *batterylevel;
    lv_obj_t *battery;
    lv_obj_t *playpause;
    lv_obj_t *call;
    lv_obj_t *distance;
    lv_obj_t *street;
    lv_obj_t *direction;
    lv_obj_t *navi;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_boot();
void tick_screen_boot();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/