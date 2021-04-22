/**
 * @file LocalTime.h
 * @author Richard Buckley <richard.buckley@ieee.org>
 * @version 1.0
 * @date 2021-04-21
 */

#pragma once

#include <string>
#include <ctime>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <utility>

/**
 * @namespace cpp_local_time
 * @brief Isolate from other code.
 */
namespace cpp_local_time {

    struct PutLocalTime;

    /**
     * @class LocalTime
     * @brief A class for displaying the local time for an arbitrary time zone.
     */
    class LocalTime {
        friend struct PutLocalTime;

    protected:
        std::string timeZoneName;   ///< The time zone to use, of empty use the system local time.
        void *timeZone;             ///< The time zone data derived from timeZoneName.
        time_t currentEpoch;        ///< The epoch to convert.
        struct tm timeStruct{};     ///< Holds the converted epoch.

    public:
        /**
         * @brief Constructor.
         * @details Creates a default LocalTime object which uses the system local time.
         */
        LocalTime() {
            timeZoneName = std::string{};
            timeZone = nullptr;
            currentEpoch = 0;
        }

        ~LocalTime();

        /**
         * @brief Check for a valid timezone data structure.
         * @details An invalid (nullptr) timezone will cause conversion to system local time.
         * @return True if time zone is valid.
         */
        explicit operator bool() const {
            return timeZone != nullptr;
        }

        /**
         * @brief Constructor.
         * @details Creates a LocalTime object for the specified time zone. Unsupported name formats
         * may cause reversion to the system localtime, or failure to load a time zone.
         * @param name The time zone name. Acceptable formats are: "GMT", "EST5EDT", ":America/Toronto".
         */
        explicit LocalTime(const std::string& name);

        /**
         * @brief Set the time epoch to the current system time.
         */
        void now() {
            currentEpoch = time(nullptr);
        }

        /**
         * @brief Get the current epoch of the object.
         * @return A time_t with the current epoch.
         */
        [[nodiscard]] time_t epoch() const {
            return currentEpoch;
        }

        /**
         * @brief Assign a time epoch to the object.
         * @param time The time to assign.
         * @return A reference to the LoacalTime.
         */
        LocalTime& operator=(time_t time) {
            currentEpoch = time;
            return *this;
        }

        /**
         * @brief Convert the epoch to a struct tm value.
         * @return True of the conversion succeeded.
         */
        bool getZoneTime();

        /**
         * @brief A stream manipulator to output the time using the specified format.
         * @param fmt A format string. See std::put_time()
         * @return The manipulator object.
         */
        template<typename S>
        PutLocalTime put(S fmt) const;
    };

    /**
     * @struct PutLocalTime
     * @brief A manipulator object to display the time on an output stream with a specified format.
     */
    struct PutLocalTime {
        const LocalTime &localTime;
        std::string fmt;
        PutLocalTime() = delete;

        [[nodiscard]] const struct tm* getTimeStruct() const { return &localTime.timeStruct; }
        PutLocalTime(const LocalTime& localTimeRef, std::string format) : localTime(localTimeRef), fmt(std::move(format)) {}
    };

    template<typename S>
    PutLocalTime LocalTime::put(S fmt) const {
        if constexpr (std::is_same_v<std::string,S>)
            return PutLocalTime(*this, fmt);
        return PutLocalTime(*this, std::string{fmt});
    }
}

/**
 * @brief Stream inserter for cpp_local_time::LocalTime.
 * @param strm An output stream.
 * @param putLocalTime A cpp_local_time::PutLocalTime structure.
 * @return The output stream.
 */
inline std::ostream& operator<<(std::ostream& strm, const cpp_local_time::PutLocalTime& putLocalTime) {
    return strm << std::put_time(putLocalTime.getTimeStruct(), putLocalTime.fmt.c_str());
}
