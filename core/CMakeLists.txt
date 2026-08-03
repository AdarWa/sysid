file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/*.hpp"
)

add_library(sysid-core ${SOURCES})

target_include_directories(sysid-core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

include(FetchContent)

FetchContent_Declare(
        Sleipnir
        GIT_REPOSITORY https://github.com/SleipnirGroup/Sleipnir.git
        GIT_TAG main
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(Sleipnir)

FetchContent_Declare(
        Eigen3
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG origin/master
)
set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(Eigen3)

set(NLOPT_CXX ON CACHE INTERNAL "")

FetchContent_Declare(
        nlopt
        GIT_REPOSITORY https://github.com/stevengj/nlopt.git
        GIT_TAG v2.11.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlopt)

if(NOT TARGET nlopt::nlopt)
    add_library(nlopt::nlopt ALIAS nlopt)
endif()

option(ENABLE_WPIUTIL "Build with wpilog support" ON)

if(ENABLE_WPIUTIL)
    include(FetchContent)

    # Disable unnecessary allwpilib modules to build only wpiutil
    set(WPILIB_WITH_CSCORE OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_GUI OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_NTCORE OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_WPIMATH OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_WPILIB OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_TESTS OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_SIMULATION_MODULES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            wpiutil
            GIT_REPOSITORY https://github.com/wpilibsuite/allwpilib.git
            GIT_TAG        main
    )

    FetchContent_MakeAvailable(wpiutil)

    add_compile_definitions(WITH_WPIUTIL)
    target_link_libraries(sysid-core PRIVATE wpiutil datalog)
endif()

target_link_libraries(sysid-core PUBLIC Sleipnir::Sleipnir Eigen3::Eigen nlopt::nlopt)