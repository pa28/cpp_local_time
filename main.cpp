#include "src/LocalTime.h"

using namespace cpp_local_time;

int main() {
    std::cout << "Hello, World!" << std::endl;

    LocalTime gmt{"GMT"};
    gmt.now();
    gmt.getZoneTime();

    LocalTime zone1{":Canada/Newfoundland"};
    zone1 = gmt.epoch();
    zone1.getZoneTime();

    LocalTime zone2{"PST8PDT"};
    zone2 = gmt.epoch();
    zone2.getZoneTime();

    LocalTime localTime;
    localTime = gmt.epoch();
    localTime.getZoneTime();

    std::string_view fmt_view{"%R:%S %Z\n"};

    auto fmt = std::string{fmt_view};

    std::cout << gmt.put(fmt_view);
    std::cout << zone1.put(fmt);
    std::cout << zone2.put(fmt);
    std::cout << localTime.put(fmt);

    return 0;
}
