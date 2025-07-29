#pragma once

#include "lvgl.hpp"
#include <memory>
#include <Preferences.h>

namespace apps {

class clock {
    public:
        clock(Preferences& prefs, lv_obj_t* parent);
        void update();
    private:
        Preferences& prefs;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> label;
        bool expanded = false;
};

}
