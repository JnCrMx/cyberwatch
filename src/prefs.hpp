#pragma once

namespace preferences {
    namespace system {
        constexpr const char* AUTO_SLEEP = "sys/auto_sleep";
        constexpr const char* AUTO_DIM = "sys/auto_dim";
        constexpr const char* HAPTIC_ENABLED = "sys/haptic_on";
        constexpr const char* BOOT_ANIMATION = "sys/boot_anim";
        constexpr const char* WAKEUP_DOUBLE_TAP = "sys/wakeup/double_tap";
        constexpr const char* WAKEUP_TILT = "sys/wakeup/tilt";
    }
    namespace apps {
        namespace clock {
            constexpr const char* EXPANDED = "clock/expanded";
        }
    }
}
