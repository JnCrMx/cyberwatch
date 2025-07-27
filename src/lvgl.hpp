#pragma once

#include <lvgl.h>

struct lv_obj_deleter {
    void operator()(lv_obj_t* obj) const {
        if (obj) {
            lv_obj_delete(obj);
        }
    }
};
