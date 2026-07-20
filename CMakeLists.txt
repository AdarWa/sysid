cmake_minimum_required(VERSION 4.1)
project(sysid)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/*.hpp"
)

add_executable(sysid ${SOURCES})

include(FetchContent)

FetchContent_Declare(
        Sleipnir
        GIT_REPOSITORY https://github.com/SleipnirGroup/Sleipnir.git
        GIT_TAG main
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(Sleipnir)

target_link_libraries(sysid PUBLIC Sleipnir::Sleipnir)

