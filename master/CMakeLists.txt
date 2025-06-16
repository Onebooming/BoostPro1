cmake_minimum_required(VERSION 3.16)
project(my_project LANGUAGES CXX)

# 强制使用C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译选项
add_compile_options(-g -Wall)

# 推荐：查找Boost库
find_package(Boost REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()
include_directories(${Boost_INCLUDE_DIRS})

# 可选：设置头文件搜索路径（根据实际项目结构调整）
include_directories(
    ${PROJECT_SOURCE_DIR}/server/src
    ${PROJECT_SOURCE_DIR}/server/src/controller
    ${PROJECT_SOURCE_DIR}/server/src/router
    ${PROJECT_SOURCE_DIR}/server/src/urldispatch
    ${PROJECT_SOURCE_DIR}/client/src
)

# Server源文件列表（便于维护，可扩展）
set(SERVER_SOURCES
    server/src/controller/compute_controller.cpp
    server/src/router/method_router.cpp
    server/src/urldispatch/url_router.cpp
    server/src/urldispatch/firststage_json_url_handler.cpp
    server/src/urldispatch/firststage_html_url_handler.cpp
    server/src/server.cpp
)

add_executable(boost_server ${SERVER_SOURCES})
target_link_libraries(boost_server PRIVATE Boost::system pthread)
target_compile_features(boost_server PRIVATE cxx_std_20)

add_executable(boost_client client/src/client.cpp)
target_link_libraries(boost_client PRIVATE Boost::system pthread)
target_compile_features(boost_client PRIVATE cxx_std_20)

# 拷贝静态资源到构建目录
add_custom_command(TARGET boost_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/server/src/static
            $<TARGET_FILE_DIR:boost_server>/static
)

# 如果客户端也需要 static，可以加上类似的命令