cmake_minimum_required(VERSION 3.12)

project(package-test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()


# test configure via cmake-config
find_package(aws-cpp-sdk-transfer REQUIRED)
# expected:
# -- Checking for module 'aws-cpp-sdk-transfer'
# --   Found aws-cpp-sdk-transfer, version 1.8.95
add_executable(${PROJECT_NAME}-cmake src/main.cc)
target_link_libraries(${PROJECT_NAME}-cmake
    aws-cpp-sdk-core
    aws-cpp-sdk-s3
    aws-cpp-sdk-transfer)
add_test(${PROJECT_NAME}-cmake ${PROJECT_NAME}-cmake)


# test configure via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(project_deps
REQUIRED
    aws-cpp-sdk-transfer
)
add_executable(${PROJECT_NAME}-pkgconfig src/main.cc)
target_link_libraries(${PROJECT_NAME}-pkgconfig ${project_deps_LIBRARIES})
add_test(${PROJECT_NAME}-pkgconfig ${PROJECT_NAME}-pkgconfig)

