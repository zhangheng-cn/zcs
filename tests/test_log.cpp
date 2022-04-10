#include "log.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "test log main \n";
    
    zcs::Logger::ptr logger(new zcs::Logger);
    LOG_DEBUG_FORMAT(logger, "%s", "my test log!");
    LOG_DEBUG_SS(logger) << "my test ss log";
    return 0;
}