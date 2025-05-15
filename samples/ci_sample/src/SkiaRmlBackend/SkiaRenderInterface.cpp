/*
 * This source file is part of Skia RmlUi Backend.
 *
 * For the latest information, see https://github.com/LibCMaker/LibCMaker_RmlUi
 *
 * Copyright (c) 2025 NikitaFeodonit
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

// Skia RmlUi Backend


#include "SkiaRenderInterface.h"

#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/Types.h>

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkSurface.h"
#include "include/core/SkVertices.h"

// #include "FileUtil.h"

#include <memory>
#include <vector>

static constexpr SkColorType COLOR_TYPE = SkColorType::kRGBA_8888_SkColorType;
static constexpr SkAlphaType ALPHA_TYPE = SkAlphaType::kPremul_SkAlphaType;
static constexpr SkColor BACKGROUND_COLOR = SK_ColorBLACK;

// static void SetRenderClipRect(SDL_Renderer* renderer, const SDL_Rect* rect)
// {
// #if SDL_MAJOR_VERSION >= 3
// 	SDL_SetRenderClipRect(renderer, rect);
// #else
// 	SDL_RenderSetClipRect(renderer, rect);
// #endif
// }
// static void SetRenderViewport(SDL_Renderer* renderer, const SDL_Rect* rect)
// {
// #if SDL_MAJOR_VERSION >= 3
// 	SDL_SetRenderViewport(renderer, rect);
// #else
// 	SDL_RenderSetViewport(renderer, rect);
// #endif
// }

SkiaRenderInterface::SkiaRenderInterface(SkCanvas* canvas)
    : canvas_ {canvas}
{
}

SkCanvas* SkiaRenderInterface::getCanvas()
{
  return canvas_;
}

void SkiaRenderInterface::BeginFrame()
{
  canvas_->clear(BACKGROUND_COLOR);
}

void SkiaRenderInterface::EndFrame() {}

Rml::CompiledGeometryHandle SkiaRenderInterface::CompileGeometry(
    Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
{
  auto* data = new GeometryView {std::move(vertices), std::move(indices)};
  return reinterpret_cast<Rml::CompiledGeometryHandle>(data);
}

void SkiaRenderInterface::RenderGeometry(
    Rml::CompiledGeometryHandle handle,
    Rml::Vector2f translation,
    Rml::TextureHandle texture)
{
  const GeometryView* geometry = reinterpret_cast<GeometryView*>(handle);
  const Rml::Vertex* vertices = geometry->vertices.data();
  const size_t vertSize = geometry->vertices.size();
  const int* indices = geometry->indices.data();
  const size_t indiSize = geometry->indices.size();

  auto* skPaint = reinterpret_cast<SkPaint*>(texture);
  SkShader* skShader = skPaint ? skPaint->getShader() : nullptr;
  SkImage* skImgage = skShader ? skShader->isAImage(nullptr, nullptr) : nullptr;

  auto skPos = std::make_unique<std::vector<SkPoint>>();
  auto skTex = std::make_unique<std::vector<SkPoint>>();
  auto skColor = std::make_unique<std::vector<SkColor>>();

  skPos->reserve(vertSize);
  skColor->reserve(vertSize);

  if(skImgage) {
    skTex->reserve(vertSize);
  }

  const int imgWidth = skImgage ? skImgage->width() : 1;
  const int imgHeight = skImgage ? skImgage->height() : 1;

  for(size_t i = 0; i < vertSize; ++i) {
    const Rml::Vertex& vert = vertices[i];
    skPos->push_back(SkPoint::Make(vert.position.x, vert.position.y));

    if(skImgage) {
      SkScalar texX = vert.tex_coord.x * imgWidth;
      SkScalar texY = vert.tex_coord.y * imgHeight;
      skTex->push_back(SkPoint::Make(texX, texY));
    }

    const Rml::ColourbPremultiplied& color = vert.colour;
    skColor->push_back(
        SkColorSetARGB(color.alpha, color.red, color.green, color.blue));
  }

  std::vector<uint16_t> skIndices;
  skIndices.reserve(indiSize);

  for(size_t i = 0; i < indiSize; ++i) {
    skIndices.push_back(static_cast<uint16_t>(indices[i]));
  }

  sk_sp<SkVertices> skVertices = SkVertices::MakeCopy(
      SkVertices::kTriangles_VertexMode, static_cast<int>(vertSize),
      skPos->data(), skTex->data(), skColor->data(), static_cast<int>(indiSize),
      skIndices.data());

  {
    SkAutoCanvasRestore acr(canvas_, true);
    canvas_->translate(translation.x, translation.y);

    // canvas_->clipRect(SkRect::MakeLTRB(drawCmd->ClipRect.x, drawCmd->ClipRect.y,
    //                                   drawCmd->ClipRect.z, drawCmd->ClipRect.w));

    // src + dst == (srcRGB * 1) + (dstRGB * (1 - srcA))
    // src + dst == (srcA * 1) + (dstA * (1 - srcA))
    // kModulate
    // kSrc, kSrcIn
    // kDstIn, kDstOut, kDstATop, kXor, kDifference, kExclusion

    if(skPaint) {
      canvas_->drawVertices(skVertices, SkBlendMode::kModulate, *skPaint);
    } else {
      canvas_->drawVertices(skVertices, SkBlendMode::kDst, SkPaint {});
    }
  }


  // SkImageInfo skImgInfo = canvas_->imageInfo();
  // std::vector<unsigned char> frameBuf(skImgInfo.computeMinByteSize());
  // canvas_->readPixels(
  //     skImgInfo, frameBuf.data(), skImgInfo.minRowBytes(), 0, 0);
  // std::string fileOutTest1 = std::to_string(handle) + "__RmlUi_draw.ppm";
  // writePpmFile(
  //     frameBuf.data(), skImgInfo.width(), skImgInfo.height(), skImgInfo.bytesPerPixel(),
  //     fileOutTest1);
}

void SkiaRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
{
  delete reinterpret_cast<GeometryView*>(geometry);
}

// Set to byte packing, or the compiler will expand our struct, which means it
// won't read correctly from file.
#pragma pack(push, 1)
struct TGAHeader
{
  char idLength;
  char colourMapType;
  char dataType;
  short int colourMapOrigin;
  short int colourMapLength;
  char colourMapDepth;
  short int xOrigin;
  short int yOrigin;
  short int width;
  short int height;
  char bitsPerPixel;
  char imageDescriptor;
};
// Restore packing.
#pragma pack(pop)

static_assert(sizeof(TGAHeader) == 18);

Rml::TextureHandle SkiaRenderInterface::LoadTexture(
    Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
  Rml::FileInterface* file_interface = Rml::GetFileInterface();
  Rml::FileHandle file_handle = file_interface->Open(source);
  if(!file_handle) {
    // TODO: Make log message for errors.
    return false;
  }

  file_interface->Seek(file_handle, 0, SEEK_END);
  size_t buffer_size = file_interface->Tell(file_handle);
  file_interface->Seek(file_handle, 0, SEEK_SET);

  if(buffer_size <= sizeof(TGAHeader)) {
    Rml::Log::Message(
        Rml::Log::LT_ERROR,
        "Texture file size is smaller than TGAHeader, file is not a valid TGA image.");
    file_interface->Close(file_handle);
    return false;
  }

  using Rml::byte;
  Rml::UniquePtr<byte[]> buffer(new byte[buffer_size]);
  size_t read_cnt =
      file_interface->Read(buffer.get(), buffer_size, file_handle);
  file_interface->Close(file_handle);

  if(read_cnt < buffer_size) {
    return false;
  }

  auto* header = reinterpret_cast<TGAHeader*>(buffer.get());

  if(header->dataType != 2) {
    Rml::Log::Message(
        Rml::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
    return false;
  }

  int color_mode = header->bitsPerPixel / 8;

  // Ensure we have at least 3 colors.
  if(color_mode < 3) {
    Rml::Log::Message(
        Rml::Log::LT_ERROR, "Only 24 and 32bit textures are supported.");
    return false;
  }

  short int& width = header->width;
  short int& height = header->height;

  // We always make 32bit textures.
  SkImageInfo skImgInfo =
      SkImageInfo::Make(width, height, COLOR_TYPE, ALPHA_TYPE);

  SkBitmap skBitmap;
  skBitmap.allocPixels(skImgInfo);

  byte* image_dest = reinterpret_cast<byte*>(skBitmap.getPixels());
  if(!image_dest) {
    return 0;
  }

  const byte* image_src = buffer.get() + sizeof(TGAHeader);

  // Targa is BGR, swap to RGB, flip Y axis, and convert to premultiplied alpha.
  for(long y = 0; y < height; ++y) {
    long read_index = y * width * color_mode;
    long write_index = ((header->imageDescriptor & 32) != 0)
        ? read_index
        : (height - y - 1) * width * 4;

    for(long x = 0; x < width; ++x) {
      image_dest[write_index] = image_src[read_index + 2];
      image_dest[write_index + 1] = image_src[read_index + 1];
      image_dest[write_index + 2] = image_src[read_index];

      if(color_mode == 4) {
        const byte alpha = image_src[read_index + 3];

        for(size_t j = 0; j < 3; j++) {
          image_dest[write_index + j] =
              byte((image_dest[write_index + j] * alpha) / 255);
        }

        image_dest[write_index + 3] = alpha;
      } else {
        image_dest[write_index + 3] = 255;
      }

      write_index += 4;
      read_index += color_mode;
    }
  }

  texture_dimensions.x = width;
  texture_dimensions.y = height;

  skBitmap.setImmutable();

  if(sk_sp<SkImage> img = skBitmap.asImage()) {
    sk_sp<SkShader> shader = img->makeShader(SkSamplingOptions {}, SkMatrix {});
    auto* paint = new SkPaint {};
    paint->setShader(shader);

    // {
    //     std::vector<unsigned char> frameBuf(skImgInfo.computeMinByteSize());
    //     img->readPixels(skImgInfo, frameBuf.data(), skImgInfo.minRowBytes(), 0, 0);
    //     std::string fileOutTest1 = std::to_string(reinterpret_cast<Rml::TextureHandle>(paint)) + "__RmlUi_draw.ppm";
    //     writePpmFile(
    //         frameBuf.data(), skImgInfo.width(), skImgInfo.height(), skImgInfo.bytesPerPixel(),
    //         fileOutTest1);
    // }

    return reinterpret_cast<Rml::TextureHandle>(paint);
  }

  return 0;
}

Rml::TextureHandle SkiaRenderInterface::GenerateTexture(
    Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
{
  Rml::Vector2i& sd = source_dimensions;
  RMLUI_ASSERT(source.data() && source.size() == size_t(sd.x * sd.y * 4));

  if(sk_sp<SkData> data = SkData::MakeWithCopy(source.data(), source.size())) {
    // We always make 32bit textures.
    SkImageInfo info = SkImageInfo::Make(sd.x, sd.y, COLOR_TYPE, ALPHA_TYPE);

    if(sk_sp<SkImage> img =
           SkImage::MakeRasterData(info, std::move(data), info.minRowBytes())) {
      sk_sp<SkShader> shader =
          img->makeShader(SkSamplingOptions {}, SkMatrix {});
      auto* paint = new SkPaint {};
      paint->setShader(shader);
      return reinterpret_cast<Rml::TextureHandle>(paint);
    }
  }

  return 0;
}

void SkiaRenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle)
{
  delete reinterpret_cast<SkPaint*>(texture_handle);
}

void SkiaRenderInterface::EnableScissorRegion(bool enable)
{
  if(enable) {
    canvas_->clipRect(rect_scissor);
  } else {
    canvas_->clipRect(SkRect::MakeEmpty());
  }

  scissor_region_enabled = enable;
}

void SkiaRenderInterface::SetScissorRegion(Rml::Rectanglei region)
{
  rect_scissor = SkRect::Make(
      {region.Left(), region.Top(), region.Right(), region.Bottom()});

  if(scissor_region_enabled) {
    canvas_->clipRect(rect_scissor);
  }
}
