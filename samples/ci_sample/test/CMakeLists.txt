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

# Testing with Google Test.
if(BUILD_TESTING)

  macro(cmr_rmlui_prepare_test_data)
    #-----------------------------------------------------------------------
    # Test data
    #-----------------------------------------------------------------------

    set(data_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/data")
    set(assets_SRC_DIR "${data_SRC_DIR}/assets")

    # Files in graphics_* differ in some bits.
    set(platform_graphics_SRC_DIR "${data_SRC_DIR}/graphics_1")

    if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux" AND
        CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(platform_graphics_SRC_DIR "${data_SRC_DIR}/graphics_2")
    endif()

    if(MSVC)
      set(platform_graphics_SRC_DIR "${data_SRC_DIR}/graphics_2")
    endif()

    if(ANDROID)
      if(NOT skia_use_fixed_gamma_text)
        if(ANDROID_ABI STREQUAL "armeabi-v7a")
          set(platform_graphics_SRC_DIR "${data_SRC_DIR}/graphics_3")
        endif()
        if(ANDROID_ABI STREQUAL "arm64-v8a")
          set(platform_graphics_SRC_DIR "${data_SRC_DIR}/graphics_2")
        endif()
        if(ANDROID_ABI STREQUAL "x86" OR ANDROID_ABI STREQUAL "x86_64")
          set(platform_graphics_SRC_DIR "${data_SRC_DIR}/graphics_1")
        endif()
      endif()
    endif()

    if(IOS)
      set(test_BIN_DIR
        "${PROJECT_BINARY_DIR}/test/$<CONFIG>-iphonesimulator/${test_NAME}.app"
      )
    else()
      set(test_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    set(assets_BIN_DIR "${test_BIN_DIR}/assets")
    set(data_BIN_DIR "${test_BIN_DIR}/data")

    if((MSVC OR XCODE) AND NOT IOS)
      set(test_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>")
    endif()


    if(TARGET Skia::icudata)
      get_target_property(icudata_LIB Skia::icudata IMPORTED_LOCATION)

      add_custom_target(prepare_icudtl_dat
        COMMAND
          ${CMAKE_COMMAND} -E make_directory "${test_BIN_DIR}"

        # `icudtl.dat` file
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${icudata_LIB}"
          "${test_BIN_DIR}"
      )

      add_dependencies(${test_NAME}
        prepare_icudtl_dat
      )
    endif()

    add_custom_target(prepare_test_assets
      COMMAND
        ${CMAKE_COMMAND} -E make_directory "${assets_BIN_DIR}"

      # Assets
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${assets_SRC_DIR}"
        "${assets_BIN_DIR}"
    )

    add_custom_target(prepare_test_data
      COMMAND
        ${CMAKE_COMMAND} -E make_directory "${data_BIN_DIR}"

      # Graphic data
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${platform_graphics_SRC_DIR}/RmlUi_draw_test_sample.ppm"
        "${data_BIN_DIR}"
    )

    add_dependencies(${test_NAME}
      prepare_test_assets
      prepare_test_data
    )
  endmacro()


  macro(cmr_rmlui_push_test_data)
    if(ANDROID)
      add_test(NAME push_assets
        COMMAND ${adb_exec} push "${assets_BIN_DIR}" "${TEST_WORK_DIR}/assets"
      )
      add_test(NAME push_data
        COMMAND ${adb_exec} push "${data_BIN_DIR}" "${TEST_WORK_DIR}/data"
      )
    endif()
  endmacro()


  macro(cmr_rmlui_pull_test_result)
    if(ANDROID)
      add_test(NAME pull_test_result
        COMMAND ${adb_exec} pull
          "${TEST_WORK_DIR}/RmlUi_draw.ppm"
          "${PROJECT_BINARY_DIR}/RmlUi_draw.ppm"
      )
    endif()
  endmacro()


  #cmr_common_test_part()
  cmr_common_test_part__set_test_name()
  cmr_common_test_part__add_executable()

  cmr_rmlui_prepare_test_data()

  cmr_common_test_part__android__init()
  cmr_common_test_part__android__push_shared_libs()

  cmr_rmlui_push_test_data()

  cmr_common_test_part__android__push_test_exe()
  cmr_common_test_part__android__run_test()

  cmr_rmlui_pull_test_result()

  cmr_common_test_part__ios()
  cmr_common_test_part__set_common_tests_properties()


  #-----------------------------------------------------------------------
  # Test sources
  #-----------------------------------------------------------------------

  target_sources(${test_NAME}
    PRIVATE
      ${test_src_DIR}/example_test.cpp
      ${test_src_DIR}/FileUtil.cpp

      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontEngineInterfaceDefault.cpp
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontEngineInterfaceDefault.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFace.cpp
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFace.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFaceHandleDefault.cpp
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFaceHandleDefault.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFaceLayer.cpp
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFaceLayer.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFamily.cpp
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontFamily.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontProvider.cpp
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontProvider.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/FontTypes.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/TextureDatabase.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/TextureLayout.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/TextureLayoutRectangle.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/TextureLayoutRow.h
      ${test_src_DIR}/SkiaRmlBackend/RmlUiFontEngineDefault/TextureLayoutTexture.h

      ${test_src_DIR}/SkiaRmlBackend/SkiaBackend.cpp
      ${test_src_DIR}/SkiaRmlBackend/SkiaBackend.h
      ${test_src_DIR}/SkiaRmlBackend/SkiaRenderInterface.cpp
      ${test_src_DIR}/SkiaRmlBackend/SkiaRenderInterface.h
      ${test_src_DIR}/SkiaRmlBackend/SkiaSystemInterface.cpp
      ${test_src_DIR}/SkiaRmlBackend/SkiaSystemInterface.h
      ${test_src_DIR}/SkiaRmlBackend/SkiaType.cpp
      ${test_src_DIR}/SkiaRmlBackend/SkiaType.h

      ${test_src_DIR}/RmlUiSamples/FileBrowser.cpp
      ${test_src_DIR}/RmlUiSamples/FileBrowser.h
      ${test_src_DIR}/RmlUiSamples/PlatformExtensions.cpp
      ${test_src_DIR}/RmlUiSamples/PlatformExtensions.h
  )

  set_target_properties(${test_NAME} PROPERTIES
    CXX_STANDARD 17
    C_STANDARD 11
  )

  target_include_directories(${test_NAME}
    PRIVATE
      ${test_src_DIR}
  )

  # RmlUi
  target_link_libraries(${test_NAME} PRIVATE RmlUi::RmlUi)

  # Skia
  target_link_libraries(${test_NAME} PRIVATE
    Skia::skshaper Skia::skunicode Skia::skia
  )
endif()
