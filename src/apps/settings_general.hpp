#pragma once

#include "lvgl.hpp"
#include <memory>
#include <Preferences.h>

namespace apps {

class settings_general {
    public:
        settings_general(Preferences& prefs, lv_obj_t* parent);
    private:
        Preferences& prefs;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> title;
};

}
