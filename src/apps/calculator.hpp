#pragma once

#include "lvgl.hpp"
#include <memory>
#include <Preferences.h>

namespace apps {

class calculator {
    public:
        calculator(Preferences& prefs, lv_obj_t* parent);
    private:
        Preferences& prefs;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> textarea;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> button;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> button_label;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> keypad;
};

}
