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
    # This is some dark magic honestly
    set(WPILIB_WITH_DOCS OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_CSCORE OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_NTCORE OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_WPICAL OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_WPIMATH OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_WPILIB OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_TESTS OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_GUI OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_FIELDS OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_SIMULATION_MODULES OFF CACHE BOOL "" FORCE)
    set(WPILIB_WITH_BENCHMARK OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            wpilib
            GIT_REPOSITORY https://github.com/wpilibsuite/allwpilib.git
            GIT_TAG        817d119e835ac13b7898a1420ea8feb835e247fc
    )

    FetchContent_MakeAvailable(wpilib)

    list(APPEND CMAKE_MODULE_PATH "${wpilib_SOURCE_DIR}/cmake/modules")
    set(WPILIB_BINARY_DIR "${wpilib_BINARY_DIR}")

    set(FILENAME_DEP_REPLACE "get_filename_component(SELF_DIR \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
    set(SELF_DIR "\${SELF_DIR}")
    set(WPIUTIL_DEP_REPLACE "find_dependency(wpiutil)")
    set(DATALOG_DEP_REPLACE "find_dependency(datalog)")

    add_subdirectory(${wpilib_SOURCE_DIR}/datalog ${wpilib_BINARY_DIR}/datalog)

    target_compile_definitions(sysid-core PUBLIC WITH_WPIUTIL)
    target_link_libraries(sysid-core PRIVATE wpiutil datalog)
endif()

target_link_libraries(sysid-core PUBLIC Sleipnir::Sleipnir Eigen3::Eigen nlopt::nlopt)