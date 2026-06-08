cmake_minimum_required (VERSION 3.15)
include(FetchContent)
# ------------------------------------------------------------------------------
# Project config
# ------------------------------------------------------------------------------
#set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TARGET_NAME "Project-Debug")
else ()
    set(TARGET_NAME "Project-Release")
endif ()
project (${TARGET_NAME} VERSION 0.0.1 LANGUAGES C CXX)

set(ASSETS_DIR "assets")
set(EXT_DIR "ext")
set(SRC_DIR "src")
include_directories(${EXT_DIR})
include_directories(${SRC_DIR})
# ------------------------------------------------------------------------------
# C version
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)
# ------------------------------------------------------------------------------
# Compile options
# ------------------------------------------------------------------------------
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDME_DEBUG=1)
else ()
    add_definitions(-DDME_NDEBUG=1)
# Enable -O2 optimization in RELEASE mode, and remove symbol tables and debugging information
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif ()

# Unicode
add_compile_definitions(UNICODE _UNICODE)
if (MSVC)
    add_compile_options("/utf-8")
# support __VA_OPT__
    add_compile_options(/Zc:preprocessor)
# Disable warning C26813
    add_compile_options(/wd26813)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
  GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(glfw)
# ------------------------------------------------------------------------------
# Set the directories for header files, source files, and static library files
# ------------------------------------------------------------------------------
#file(GLOB_RECURSE HEAD_FILES "${SRC_DIR}/*.h" "${SRC_DIR}/*.hpp")
file(GLOB_RECURSE SRC_FILES "${EXT_DIR}/*.c" "${EXT_DIR}/*.cpp" "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")
# ------------------------------------------------------------------------------
# Set executable files
# ------------------------------------------------------------------------------
add_executable(${TARGET_NAME} ${SRC_FILES})
target_link_libraries(${TARGET_NAME} PRIVATE glfw)
# ------------------------------------------------------------------------------
# Auto update assets files
# ------------------------------------------------------------------------------
# 1. Set assets and output directory
set(RES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_DIR}")
set(RES_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${ASSETS_DIR}")

# 2. Recursively obtain all files
file(GLOB_RECURSE ALL_RES_FILES RELATIVE "${RES_SOURCE_DIR}" "${RES_SOURCE_DIR}/*")
# 3. Configure copy rules for each files
set(UPDATED_RES_FILES "")
foreach(RES_PATH ${ALL_RES_FILES})
    set(SRC "${RES_SOURCE_DIR}/${RES_PATH}")
    set(DST "${RES_OUTPUT_DIR}/${RES_PATH}")

    # add_custom_command will automatically create the parent directory required by DST
    add_custom_command(
        OUTPUT "${DST}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SRC}" "${DST}"
        DEPENDS "${SRC}"
        COMMENT "Copying resource: ${RES_PATH}"
    )
    list(APPEND UPDATED_RES_FILES "${DST}")
endforeach()

# 4. Create a custom target and associate it with the program
add_custom_target(UpdateAssets ALL DEPENDS ${UPDATED_RES_FILES})
add_dependencies(${TARGET_NAME} UpdateAssets)
# ------------------------------------------------------------------------------