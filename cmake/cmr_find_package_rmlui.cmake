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

# Part of "LibCMaker/cmake/cmr_find_package.cmake".

  #-----------------------------------------------------------------------
  # Library specific build arguments
  #-----------------------------------------------------------------------

## +++ Common part of the lib_cmaker_<lib_name> function +++
  set(find_LIB_VARS
    COPY_RMLUI_CMAKE_BUILD_SCRIPTS

    #BUILD_SHARED_LIBS
    RMLUI_SAMPLES
    RMLUI_BACKEND
    RMLUI_FONT_ENGINE
    #BUILD_TESTING
    RMLUI_LUA_BINDINGS
    RMLUI_LOTTIE_PLUGIN
    RMLUI_SVG_PLUGIN
    RMLUI_HARFBUZZ_SAMPLE
    RMLUI_THIRDPARTY_CONTAINERS
    RMLUI_MATRIX_ROW_MAJOR
    RMLUI_CUSTOM_RTTI
    RMLUI_PRECOMPILED_HEADERS
    RMLUI_COMPILER_OPTIONS
    RMLUI_WARNINGS_AS_ERRORS
    RMLUI_TRACY_PROFILING
    RMLUI_TRACY_MEMORY_PROFILING
    RMLUI_TRACY_CONFIGURATION
    RMLUI_CUSTOM_CONFIGURATION
    RMLUI_CUSTOM_CONFIGURATION_FILE
    RMLUI_CUSTOM_INCLUDE_DIRS
    RMLUI_CUSTOM_LINK_LIBRARIES
    RMLUI_INSTALL_TARGETS_DIR
    RMLUI_INSTALL_LICENSES_AND_BUILD_INFO
  )

  foreach(d ${find_LIB_VARS})
    if(DEFINED ${d})
      list(APPEND find_CMAKE_ARGS
        -D${d}=${${d}}
      )
    endif()
  endforeach()
## --- Common part of the lib_cmaker_<lib_name> function ---


  #-----------------------------------------------------------------------
  # Building
  #-----------------------------------------------------------------------

## +++ Common part of the lib_cmaker_<lib_name> function +++
  cmr_lib_cmaker_main(
    LibCMaker_DIR ${find_LibCMaker_DIR}
    NAME          ${find_NAME}
    VERSION       ${find_VERSION}
    LANGUAGES     CXX C
    BASE_DIR      ${find_LIB_DIR}
    DOWNLOAD_DIR  ${cmr_DOWNLOAD_DIR}
    UNPACKED_DIR  ${cmr_UNPACKED_DIR}
    BUILD_DIR     ${lib_BUILD_DIR}
    CMAKE_ARGS    ${find_CMAKE_ARGS}
    INSTALL
  )
## --- Common part of the lib_cmaker_<lib_name> function ---
