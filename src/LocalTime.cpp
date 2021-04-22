/**
 * @file LocalTime.cpp
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 2021-04-21
 */

#include "LocalTime.h"
extern "C" {
#include "../tzfiles/private.h"
}
#undef bool     // tzfiles/private.h defines bool as int :/

namespace cpp_local_time {
    LocalTime::~LocalTime() {
        if (timeZone)
            tzfree(static_cast<timezone_t>(timeZone));
    }

    LocalTime::LocalTime(const std::string& name) : LocalTime() {
        if (!name.empty()) {
            timeZone = static_cast<void*>(tzalloc(name.c_str()));
            timeZoneName = name;
        }
    }

    bool LocalTime::getZoneTime() {
        if (timeZone)
            return localtime_rz(static_cast<timezone_t>(timeZone), &currentEpoch, &timeStruct) != nullptr;
        return localtime_r(&currentEpoch, &timeStruct) != nullptr;
    }

    PutLocalTime LocalTime::put(const char *fmt) const {
        return PutLocalTime{*this,fmt};
    }

    PutLocalTime LocalTime::put(const std::string &fmt) const {
        return put(fmt.c_str());
    }

    PutLocalTime LocalTime::put(const std::string_view fmt) const {
        return put(std::string{fmt}.c_str());
    }
}
