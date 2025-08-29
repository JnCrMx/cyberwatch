#pragma once

#include "lvgl.hpp"
#include <memory>
#include <Preferences.h>

namespace apps {

class settings_datetime {
    public:
        settings_datetime(Preferences& prefs, lv_obj_t* parent);
    private:
        Preferences& prefs;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> title;

        lv_obj_t* hour;
        lv_obj_t* minute;
        lv_obj_t* second;

        lv_obj_t* parent_tile;
        bool open = false;
        bool dirty = false;
};

}
