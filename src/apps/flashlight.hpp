#pragma once

#include "lvgl.hpp"
#include <memory>
#include <Preferences.h>

namespace apps {

class flashlight {
    public:
        flashlight(Preferences& prefs, lv_obj_t* parent);
};

}
