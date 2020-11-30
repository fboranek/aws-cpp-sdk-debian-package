cmake_minimum_required(VERSION 3.12)
include(FetchContent)


project(aws-sdk-cpp-build LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)  # Debian 10 (buster) and higher
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(FETCHCONTENT_QUIET OFF)
set(CMAKE_BUILD_TYPE RELEASE CACHE BOOL "Force build type" FORCE)


# debug
message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")



# read sdk version from changelog into variable sdk_VERSION
execute_process(
    COMMAND dpkg-parsechangelog -S Version
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE changelog_VERSION_RESULT
    ERROR_VARIABLE changelog_VERSION_ERROR
    OUTPUT_VARIABLE changelog_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS debian/changelog)

if (${changelog_VERSION_RESULT} EQUAL 0)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" sdk_VERSION ${changelog_VERSION})
    message(STATUS "Parsed sdk version ${sdk_VERSION}")
else()
    message(FATAL_ERROR "Failed parsing version: ${changelog_VERSION_ERROR}")
endif()

# upstrem projet settings
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Set appriory project value" FORCE)
set(CPP_STANDARD ${CMAKE_CXX_STANDARD} CACHE STRING "Set appriory project value" FORCE)
set(SIMPLE_INSTALL ON CACHE BOOL "Set appriory project value" FORCE)
# third party dependencies are build and copied to default system directory during SDK build, so it needs root access
# and it makes harder to create package for these third party library. Set dedicated install dir for them.
set(AWS_DEPS_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/aws_deps" CACHE PATH "Dependencies install directory.")
set(BUILD_ONLY
    "core"
    "s3"
    "transfer"
    CACHE STRING "Set appriory project value" FORCE)

# upstream project
FetchContent_Declare(
    sdk
    GIT_REPOSITORY https://github.com/aws/aws-sdk-cpp
    GIT_TAG        ${sdk_VERSION}
)
FetchContent_GetProperties(sdk)
if(NOT sdk_POPULATED)
    FetchContent_Populate(sdk)
    message(STATUS "sdk_POPULATED=${sdk_POPULATED} sdk_SOURCE_DIR = ${sdk_SOURCE_DIR} sdk_BINARY_DIR=${sdk_BINARY_DIR}")
    if(NOT sdk_POPULATED)
        message(FATAL_ERROR "Failed to build aws-sdk-cpp from source")
    endif()
    add_subdirectory("${sdk_SOURCE_DIR}" "${sdk_BINARY_DIR}")
endif()

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/aws_deps/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/aws_deps/lib/ DESTINATION ${CMAKE_INSTALL_LIBDIR})
