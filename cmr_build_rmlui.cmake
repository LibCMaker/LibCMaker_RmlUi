# ****************************************************************************
#  Project:  LibCMaker
#  Purpose:  A CMake build scripts for build libraries with CMake
#  Author:   NikitaFeodonit, nfeodonit@yandex.com
# ****************************************************************************
#    Copyright (c) 2017-2025 NikitaFeodonit
#
#    This file is part of the LibCMaker project.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published
#    by the Free Software Foundation, either version 3 of the License,
#    or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#    See the GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program. If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************

#-----------------------------------------------------------------------
# The file is an example of the convenient script for the library build.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Lib's name, version, paths
#-----------------------------------------------------------------------

set(RMLUI_lib_NAME "RmlUi")
set(RMLUI_lib_VERSION "6.1" CACHE STRING "RMLUI_lib_VERSION")
set(RMLUI_lib_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "RMLUI_lib_DIR")


# To use our Find<LibName>.cmake.
list(APPEND CMAKE_MODULE_PATH "${RMLUI_lib_DIR}/cmake/modules")


#-----------------------------------------------------------------------
# LibCMaker_<LibName> specific vars and options
#-----------------------------------------------------------------------

option(COPY_RMLUI_CMAKE_BUILD_SCRIPTS "COPY_RMLUI_CMAKE_BUILD_SCRIPTS" ON)


#-----------------------------------------------------------------------
# Library specific vars and options
#-----------------------------------------------------------------------

# Add lists of available options for those that support it
include("${RMLUI_lib_DIR}/cmake/OptionsLists.cmake")


option(BUILD_SHARED_LIBS "CMake standard option. Choose whether to build shared RmlUi libraries." ON)

option(RMLUI_SAMPLES "Build samples of the library." OFF)

set(RMLUI_BACKEND "auto" CACHE STRING "Backend to use when building the RmlUi samples. Choose one from ./CMake/OptionsLists.cmake.")
set_property(CACHE RMLUI_BACKEND PROPERTY STRINGS ${RMLUI_BACKEND_OPTIONS})
if(NOT RMLUI_BACKEND IN_LIST RMLUI_BACKEND_OPTIONS)
    message(FATAL_ERROR "The RmlUi backend '${RMLUI_BACKEND}' specified in RMLUI_BACKEND is not valid. Available options: ${RMLUI_BACKEND_OPTIONS}")
endif()

set(RMLUI_FONT_ENGINE "none" CACHE STRING "Font engine to build RmlUi with. If set to \"none\", RmlUi won't build with any of the included font engines and some samples might be disabled.")
set_property(CACHE RMLUI_FONT_ENGINE PROPERTY STRINGS ${RMLUI_FONT_ENGINE_OPTIONS})
if(NOT RMLUI_FONT_ENGINE IN_LIST RMLUI_FONT_ENGINE_OPTIONS)
    message(FATAL_ERROR "The RmlUi font engine '${RMLUI_FONT_ENGINE}' specified in RMLUI_FONT_ENGINE is not valid. Available options: ${RMLUI_FONT_ENGINE_OPTIONS}")
endif()

option(BUILD_TESTING "CMake standard option. Enable RmlUi testing projects." OFF)

option(RMLUI_LUA_BINDINGS "Build Lua bindings." OFF)

option(RMLUI_LOTTIE_PLUGIN "Enable plugin for Lottie animations. Requires the rlottie library." OFF)
option(RMLUI_SVG_PLUGIN "Enable plugin for SVG images. Requires the lunasvg library." OFF)

option(RMLUI_HARFBUZZ_SAMPLE "Enable harfbuzz text shaping sample. Requires the harfbuzz library." OFF)

option(RMLUI_THIRDPARTY_CONTAINERS "Enable integrated third-party containers for improved performance, rather than their standard library counterparts." ON)

option(RMLUI_MATRIX_ROW_MAJOR "Use row-major matrices. Column-major matrices are used by default." OFF)

option(RMLUI_CUSTOM_RTTI "Build RmlUi with a custom implementation of run-time type information (RTTI). When enabled, all usage of language RTTI features such as dynamic_cast will be disabled." OFF)

option(RMLUI_PRECOMPILED_HEADERS "Enable precompiled headers for RmlUi." ON)

option(RMLUI_COMPILER_OPTIONS "Enable recommended compiler-specific options for the project, such as for supported warning level, standards conformance, and multiprocess builds. Turn off for full control over compiler flags." OFF)

option(RMLUI_WARNINGS_AS_ERRORS "Treat compiler warnings as errors." OFF)

option(RMLUI_TRACY_PROFILING "Enable profiling with Tracy. Source files can optionally be placed in `Dependencies/tracy`." OFF)
if(RMLUI_TRACY_PROFILING)
    option(RMLUI_TRACY_MEMORY_PROFILING "Overload global operator new/delete to track memory allocations in Tracy." ON)
endif()
if(RMLUI_TRACY_PROFILING AND CMAKE_CONFIGURATION_TYPES)
    option(RMLUI_TRACY_CONFIGURATION "Enable a separate Tracy configuration type for multi-config generators such as Visual Studio, otherwise enable Tracy in all configurations." ON)
endif()

option(RMLUI_CUSTOM_CONFIGURATION "Customize the RmlUi configuration file to override the default configuration and types." OFF)
set(RMLUI_CUSTOM_CONFIGURATION_FILE "" CACHE STRING "Custom configuration file to be included in place of <RmlUi/Config/Config.h>.")
set(RMLUI_CUSTOM_INCLUDE_DIRS "" CACHE STRING "Extra include directories (use with RMLUI_CUSTOM_CONFIGURATION_FILE).")
set(RMLUI_CUSTOM_LINK_LIBRARIES "" CACHE STRING "Extra link libraries (use with RMLUI_CUSTOM_CONFIGURATION_FILE).")

set(RMLUI_INSTALL_TARGETS_DIR "" CACHE STRING "Override the install directory for the generated CMake targets.")
option(RMLUI_INSTALL_LICENSES_AND_BUILD_INFO "Install license files and build info intended for the github actions workflow." OFF)


#-----------------------------------------------------------------------
# Build, install and find the library
#-----------------------------------------------------------------------

cmr_find_package(
  LibCMaker_DIR   ${LibCMaker_DIR}
  NAME            ${RMLUI_lib_NAME}
  VERSION         ${RMLUI_lib_VERSION}
  LIB_DIR         ${RMLUI_lib_DIR}
  REQUIRED
  CONFIG
)
