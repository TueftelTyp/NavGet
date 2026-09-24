#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_battery;
extern const lv_img_dsc_t img_battlevel;
extern const lv_img_dsc_t img_play;
extern const lv_img_dsc_t img_pause;
extern const lv_img_dsc_t img_incoming;
extern const lv_img_dsc_t img_outgoing;
extern const lv_img_dsc_t img_ended;
extern const lv_img_dsc_t img_arrow;
extern const lv_img_dsc_t img_01_straight;
extern const lv_img_dsc_t img_02_slight_left;
extern const lv_img_dsc_t img_03_left;
extern const lv_img_dsc_t img_04_sharp_left;
extern const lv_img_dsc_t img_05_slight_right;
extern const lv_img_dsc_t img_06_right;
extern const lv_img_dsc_t img_07_sharp_right;
extern const lv_img_dsc_t img_08_u_turn_left;
extern const lv_img_dsc_t img_09_u_turn_right;
extern const lv_img_dsc_t img_10_roundabout_right;
extern const lv_img_dsc_t img_11_roundabout_upper_right;
extern const lv_img_dsc_t img_12_roundabout_straight;
extern const lv_img_dsc_t img_13_roundabout_upper_left;
extern const lv_img_dsc_t img_14_roundabout_left;
extern const lv_img_dsc_t img_15_roundabout_lower_left;
extern const lv_img_dsc_t img_16_roundabout_lower_right;
extern const lv_img_dsc_t img_17_roundabout_u_turn;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[25];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/