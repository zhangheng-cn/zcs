# 默认创建的变量名
# PROJECT_SOURCE_DIR //本CMakeLists.txt 所在的目录
# PROJECT_NAME  

cmake_minimum_required(VERSION 3.4)

project(zcs)

# add_definitions()

# 路径下的所有.cpp .cc .c 赋值给SRC
aux_source_directory(src SRC)

# 将文件生成.a/.so
# add_library(ZNET STATIC/SHARED ${SRC})

#set (SRC
#
#)

include_directories(src)

#add_executable(zcs ${SRC})

add_executable(test_log "tests/test_log.cpp" ${SRC})
add_executable(test_timer "tests/test_timer.cpp" ${SRC})
#target_link_libraries(zcs pthread)

