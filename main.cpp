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

    std::cout << gmt.put("%R:%S %Z\n");
    std::cout << zone1.put("%R:%S %Z\n");
    std::cout << zone2.put("%R:%S %Z\n");
    std::cout << localTime.put("%R:%S %Z\n");

    return 0;
}
