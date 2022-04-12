# 默认创建的变量名
# PROJECT_SOURCE_DIR //本CMakeLists.txt 所在的目录
# PROJECT_NAME  

cmake_minimum_required(VERSION 3.4)

project(zcs)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

# add_definitions()

# 路径下的所有.cpp .cc .c 赋值给SRC
aux_source_directory(src SRC)

# 将文件生成.a/.so
add_library(znet STATIC ${SRC})

set(LIBS
    znet
    pthread
)
#set (SRC
#
#)

include_directories(src)

#add_executable(zcs ${SRC})
add_executable(test_log "tests/test_log.cpp")
target_link_libraries(test_log ${LIBS})

add_executable(test_timer "tests/test_timer.cpp")
target_link_libraries(test_timer ${LIBS})

add_executable(test_thread "tests/test_thread.cpp")
target_link_libraries(test_thread ${LIBS})
#target_link_libraries(zcs pthread)

