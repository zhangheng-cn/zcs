#include "zcs.h"
#include <stdio.h>
#include <assert.h>

zcs::Logger::ptr g_logger = ZCS_LOG_ROOT();

void test_assert() {
    ZCS_ASSERT(false);
    ZCS_ASSERT2(false, "xxxxxx");
}


int main(int argc, char* argv[]) {
    
    test_assert();

    return 0;
}