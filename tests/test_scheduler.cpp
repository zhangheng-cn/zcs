#include "zcs.h"
#include <iostream>


zcs::Logger::ptr g_logger = ZCS_LOG_NAME("test");

void test_sc() {
    int i = 0;
    while(i < 2) {
        ZCS_DEBUG(g_logger) << "fiber running!";
        zcs::Fiber::GetThis()->YieldToHold();
        sleep(1);
        i++;
    }
}

int main(int argc, char* argv[]) {


    return 0;
}