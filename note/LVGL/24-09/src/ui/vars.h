#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_DISTANCE = 0,
    FLOW_GLOBAL_VARIABLE_BATTERYLEVEL = 1,
    FLOW_GLOBAL_VARIABLE_STREET = 2,
    FLOW_GLOBAL_VARIABLE_DIRECTION = 3
};

// Native global variables

extern int32_t get_var_distance();
extern void set_var_distance(int32_t value);
extern int32_t get_var_batterylevel();
extern void set_var_batterylevel(int32_t value);
extern const char *get_var_street();
extern void set_var_street(const char *value);
extern float get_var_direction();
extern void set_var_direction(float value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/