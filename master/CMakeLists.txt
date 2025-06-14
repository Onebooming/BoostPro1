cmake_minimum_required(VERSION 3.16)
project(my_project CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-g -Wall)

add_executable(boost_server server/src/controller/compute_controller.cpp  server/src/router/method_router.cpp server/src/server.cpp)
target_link_libraries(boost_server boost_system pthread)
add_executable(boost_client client/src/client.cpp)
target_link_libraries(boost_client boost_system pthread)

file(COPY server/src/static DESTINATION ${CMAKE_BINARY_DIR})