#pragma once

#include <nvs.h>
#include <string>

namespace preferences {
    namespace detail {
        template<size_t N>
        constexpr const char* validate_key(char const (&key)[N]) {
            static_assert(N < NVS_KEY_NAME_MAX_SIZE, "Key name too long");
            return key;
        }
    }

    namespace system {
        constexpr const char* AUTO_SLEEP = detail::validate_key("sys/auto_sleep");
        constexpr const char* AUTO_DIM = detail::validate_key("sys/auto_dim");
        constexpr const char* HAPTIC_ENABLED = detail::validate_key("sys/haptic_on");
        constexpr const char* BOOT_ANIMATION = detail::validate_key("sys/boot_anim");
        constexpr const char* WAKEUP_DOUBLE_TAP = detail::validate_key("sys/wake/2tap");
        constexpr const char* WAKEUP_TILT = detail::validate_key("sys/wake/tilt");
    }
    namespace apps {
        namespace clock {
            constexpr const char* EXPANDED = detail::validate_key("clock/expanded");
        }
    }
}
