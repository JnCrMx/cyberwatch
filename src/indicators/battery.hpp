#pragma once

#include "lvgl.hpp"
#include <memory>
#include <Preferences.h>

namespace indicators {

class battery {
    public:
        battery(Preferences& prefs, lv_obj_t* parent);
        void update();

        lv_obj_t* obj() {
            return container.get();
        }
    private:
        Preferences& prefs;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> container;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> bar;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> charging_icon;
        std::unique_ptr<lv_obj_t, lv_obj_deleter> percentage_label;
};

}
