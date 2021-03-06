# 默认创建的变量名
# PROJECT_SOURCE_DIR //本CMakeLists.txt 所在的目录
# PROJECT_NAME  

cmake_minimum_required(VERSION 3.4)

project(zcs)
include (cmake/utils.cmake)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

# add_definitions()

# 路径下的所有.cpp .cc .c 赋值给SRC
aux_source_directory(src SRC)

# 将文件生成.a/.so
add_library(znet STATIC ${SRC})
force_redefine_file_macro_for_sources (znet)

set(LIBS
    znet
    pthread
    yaml-cpp
    #socket
)
#set (SRC
#
#)

include_directories(src)
include_directories(src/http)


#add_executable(zcs ${SRC})
add_executable(test_log "tests/test_log.cpp")
target_link_libraries(test_log ${LIBS})
force_redefine_file_macro_for_sources (test_log)

add_executable(test_timer "tests/test_timer.cpp")
target_link_libraries(test_timer ${LIBS})
force_redefine_file_macro_for_sources (test_timer)

add_executable(test_thread "tests/test_thread.cpp")
target_link_libraries(test_thread ${LIBS})
force_redefine_file_macro_for_sources (test_thread)

add_executable(test_server "tests/test_server.cpp" "tests/http.cpp" )
target_link_libraries(test_server ${LIBS})
force_redefine_file_macro_for_sources (test_server)

add_executable(test_thread_pool "tests/test_thread_pool.cpp")
target_link_libraries(test_thread_pool ${LIBS})
#target_link_libraries(zcs pthread)
force_redefine_file_macro_for_sources (test_thread_pool)

add_executable(test_config "tests/test_config.cpp")
target_link_libraries(test_config ${LIBS})
force_redefine_file_macro_for_sources (test_config)

add_executable(test_util "tests/test_util.cpp")
target_link_libraries(test_util ${LIBS})
force_redefine_file_macro_for_sources (test_util)

add_executable(test_fiber "tests/test_fiber.cpp")
target_link_libraries(test_fiber ${LIBS})
force_redefine_file_macro_for_sources (test_fiber)

add_executable(test_scheduler "tests/test_scheduler.cpp")
target_link_libraries(test_scheduler ${LIBS})
force_redefine_file_macro_for_sources (test_scheduler)

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)