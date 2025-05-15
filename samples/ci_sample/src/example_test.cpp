/*****************************************************************************
 * Project:  LibCMaker
 * Purpose:  A CMake build scripts for build libraries with CMake
 * Author:   NikitaFeodonit, nfeodonit@yandex.com
 *****************************************************************************
 *   Copyright (c) 2017-2025 NikitaFeodonit
 *
 *    This file is part of the LibCMaker project.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published
 *    by the Free Software Foundation, either version 3 of the License,
 *    or (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *    See the GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include <SkiaRmlBackend/SkiaBackend.h>
#include <SkiaRmlBackend/SkiaRenderInterface.h>

#include <RmlUi/Core.h>

// #include <RmlUiSamples/PlatformExtensions.h>
// #include "RmlUiSamples/FileBrowser.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/core/SkSurface.h"

#include "FileUtil.h"

#include <stdio.h>
#include <vector>

#include "gtest/gtest.h"


#if defined(_WIN32) || defined(WIN32)
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif


// https://stackoverflow.com/a/5920028
#if __APPLE__
#include <TargetConditionals.h>
#include <string.h>

#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#include <CoreFoundation/CFBundle.h>
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#endif


// https://stackoverflow.com/a/5920028
#if __APPLE__
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator

// https://stackoverflow.com/a/67022872
std::string get_resources_dir()
{
  CFURLRef resourceURL =
      CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
  char resourcePath[PATH_MAX];

  if(CFURLGetFileSystemRepresentation(
         resourceURL, true, (UInt8*) resourcePath, PATH_MAX)) {
    if(resourceURL != NULL) {
      CFRelease(resourceURL);
    }
    return resourcePath;
  }
}
#endif
#endif  // __APPLE__


// == Start import of code from Samples/shell/src/Shell.cpp ==

static void LoadFonts(std::string resourceDir)
{
  const Rml::String directory = resourceDir + "assets" + PATH_SEPARATOR;

  struct FontFace
  {
    const char* filename;
    bool fallback_face;
  };

  FontFace font_faces[] = {
      {"LatoLatin-Regular.ttf", false}, {"LatoLatin-Italic.ttf", false},
      {"LatoLatin-Bold.ttf", false},    {"LatoLatin-BoldItalic.ttf", false},
      {"NotoEmoji-Regular.ttf", true},
  };

  for(const FontFace& face : font_faces) {
    Rml::LoadFontFace(directory + face.filename, face.fallback_face);
  }
}

// == End import of code from Samples/shell/src/Shell.cpp ==


// == Start import of code from Samples/basic/tree_view/src/FileBrowser.cpp ==

struct File
{
  File(bool directory, int depth, Rml::String name)
      : visible(depth == 0)
      , directory(directory)
      , collapsed(true)
      , depth(depth)
      , name(std::move(name))
  {
  }

  bool visible;
  bool directory;
  bool collapsed;
  int depth;
  Rml::String name;
};

static Rml::Vector<File> files;

static void ToggleExpand(
    Rml::DataModelHandle handle,
    Rml::Event& /*ev*/,
    const Rml::VariantList& parameters)
{
  if(parameters.empty())
    return;

  // The index of the file/directory being toggled is passed in as the first parameter.
  const size_t toggle_index = (size_t) parameters[0].Get<int>();
  if(toggle_index >= files.size())
    return;

  File& toggle_file = files[toggle_index];

  const bool collapsing = !toggle_file.collapsed;
  const int depth = toggle_file.depth;

  toggle_file.collapsed = collapsing;

  // Loop through all descendent entries.
  for(size_t i = toggle_index + 1; i < files.size() && files[i].depth > depth;
      i++) {
    // Hide all items if we are collapsing. If instead we are expanding, make all direct children visible.
    files[i].visible = !collapsing && (files[i].depth == depth + 1);
    files[i].collapsed = true;
  }

  handle.DirtyVariable("files");
}

// == End import of code from Samples/basic/tree_view/src/FileBrowser.cpp ==


bool DataSourceInitialise(Rml::Context* context)
{
  int current_depth = 0;

  files.push_back(File(true, current_depth, "Dir_1"));
  files.push_back(File(true, current_depth, "Dir_2"));
  files.push_back(File(true, current_depth, "Dir_3"));

  files.push_back(File(false, current_depth, "file_name_1"));
  files.push_back(File(false, current_depth, "file_name_2"));
  files.push_back(File(false, current_depth, "file_name_3"));
  files.push_back(File(false, current_depth, "file_name_4"));

  Rml::DataModelConstructor constructor =
      context->CreateDataModel("filebrowser");

  if(!constructor)
    return false;

  if(auto file_handle = constructor.RegisterStruct<File>()) {
    bool res = true;
    res = res && file_handle.RegisterMember("visible", &File::visible);
    res = res && file_handle.RegisterMember("directory", &File::directory);
    res = res && file_handle.RegisterMember("collapsed", &File::collapsed);
    res = res && file_handle.RegisterMember("depth", &File::depth);
    res = res && file_handle.RegisterMember("name", &File::name);
    if(!res) {
      return false;
    }
  }

  if(!constructor.RegisterArray<decltype(files)>()) {
    return false;
  }

  if(!constructor.Bind("files", &files)) {
    return false;
  }

  if(!constructor.BindEventCallback("toggle_expand", &ToggleExpand)) {
    return false;
  }

  return true;
}


TEST(Example, test_RmlUi)
{
  std::string resourceDir;
  std::string workDir;

// https://stackoverflow.com/a/5920028
#if __APPLE__
#if TARGET_IPHONE_SIMULATOR
  // iOS Simulator

  resDir = get_resources_dir() + "/";

  // https://stackoverflow.com/a/39022407
  // HOME is the home directory of your application
  // points to the root of your sandbox.
  std::string homeDir = getenv("HOME");
  workDir = homeDir + "/Documents/";
#endif
#endif  // __APPLE__


  std::string fileOutTest1 = workDir + "RmlUi_draw.ppm";
  remove(fileOutTest1.c_str());


  const int frameWidth = 490;
  const int frameHeight = 500;

  // Constructs the system and render interfaces, creates a window, and attaches the renderer.
  if(!SkiaBackend::Initialize(
         "Load Document Sample", frameWidth, frameHeight, true)) {
    ASSERT_TRUE(false);
  }

  // Install the custom interfaces constructed by the backend before initializing RmlUi.
  Rml::SetSystemInterface(SkiaBackend::GetSystemInterface());
  Rml::SetFontEngineInterface(SkiaBackend::GetFontEngineInterface());
  SkiaRenderInterface* skRmlRender =
      static_cast<SkiaRenderInterface*>(SkiaBackend::GetRenderInterface());
  Rml::SetRenderInterface(skRmlRender);

  // RmlUi initialisation.
  if(!Rml::Initialise()) {
    SkiaBackend::Shutdown();
    ASSERT_TRUE(false);
  }

  // Create the main RmlUi context.
  Rml::Context* rmlContext =
      Rml::CreateContext("main", Rml::Vector2i(frameWidth, frameHeight));
  if(!rmlContext) {
    Rml::Shutdown();
    SkiaBackend::Shutdown();
    ASSERT_TRUE(false);
  }

  // Fonts should be loaded before any documents are loaded.
  LoadFonts(resourceDir);

  // Create the file data source and formatter.
  // const Rml::String root = PlatformExtensions::FindSamplesRoot();
  // ASSERT_TRUE(FileBrowser::Initialise(rmlContext, root));
  ASSERT_TRUE(DataSourceInitialise(rmlContext));

  // Load and show the demo document.
  const Rml::String documentPath = "assets/tree_view.rml";

  if(Rml::ElementDocument* document = rmlContext->LoadDocument(documentPath)) {
    document->GetElementById("title")->SetInnerRML(document->GetTitle());
    document->Show();
  }

  // Always update the context before rendering.
  rmlContext->Update();

  // Prepare the backend for taking rendering commands from RmlUi and then render the context.
  SkiaBackend::BeginFrame();
  rmlContext->Render();
  SkiaBackend::PresentFrame();


  // Get picture from SkCanvas.
  SkCanvas* skCanvas = skRmlRender->getCanvas();
  SkImageInfo skImgInfo = skCanvas->imageInfo();
  std::vector<unsigned char> frameBuf(skImgInfo.computeMinByteSize());

  EXPECT_TRUE(skCanvas->readPixels(
      skImgInfo, frameBuf.data(), skImgInfo.minRowBytes(), 0, 0));

  // Write picture to file.
  writePpmFile(
      frameBuf.data(), skImgInfo.width(), skImgInfo.height(),
      skImgInfo.bytesPerPixel(), fileOutTest1);

  // Compare our file with prototype.
  std::string fileTest1 =
      resourceDir + "data" + PATH_SEPARATOR + "RmlUi_draw_test_sample.ppm";
  EXPECT_TRUE(compareFiles(fileTest1, fileOutTest1));


  // Shutdown RmlUi.
  Rml::Shutdown();
  SkiaBackend::Shutdown();
}
