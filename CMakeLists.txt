cmake_minimum_required(VERSION 3.13)



################################################################################
## PROJECT
## name and version
################################################################################
project(PanTilt LANGUAGES CXX)



################################################################################
## SETTINGS
## basic project settings before use
################################################################################
# If this project is used as a submodule, the variable should be overridden
# to "OFF" in the top-level application (to disable forced cache rewriting)
option(${PARENT}_SUBMODULE_CACHE_OVERWRITE "Enable forced cache rewriting" ON)
if (${PARENT}_SUBMODULE_CACHE_OVERWRITE)
    SET(REWRITE_FORCE "FORCE")
else()
    SET(REWRITE_FORCE "")
endif()

# Set default Release mode if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type defaulting to Release" FORCE)
endif()



################################################################################
## CONFIGURATION
## project configuration
################################################################################
SET(${PARENT}_PAN_TILT                       ON  CACHE BOOL "" ${REWRITE_FORCE})
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    SET(${PARENT}_PAN_TILT_TEST              OFF CACHE BOOL "" ${REWRITE_FORCE})
    SET(${PARENT}_PAN_TILT_EXAMPLE           OFF CACHE BOOL "" ${REWRITE_FORCE})
    message("${PROJECT_NAME} included as subrepository.")
else()
    SET(${PARENT}_PAN_TILT_TEST              ON  CACHE BOOL "" ${REWRITE_FORCE})
    SET(${PARENT}_PAN_TILT_EXAMPLE           ON  CACHE BOOL "" ${REWRITE_FORCE})
    message("${PROJECT_NAME} is a standalone project.")
endif()



################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the project configuration
################################################################################
if (${PARENT}_PAN_TILT)
    add_subdirectory(3rdparty)
    add_subdirectory(src)
endif()

if (${PARENT}_PAN_TILT_TEST)
    add_subdirectory(test)
endif()

if (${PARENT}_PAN_TILT_EXAMPLE)
    add_subdirectory(example)
endif()