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

target_link_libraries(sysid-core PUBLIC Sleipnir::Sleipnir Eigen3::Eigen nlopt::nlopt)