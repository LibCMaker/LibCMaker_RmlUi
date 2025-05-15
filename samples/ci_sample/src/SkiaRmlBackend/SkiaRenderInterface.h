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


#ifndef SKIARMLBACKEND_SKIARENDERINTERFACE_H
#define SKIARMLBACKEND_SKIARENDERINTERFACE_H

#include <RmlUi/Core/RenderInterface.h>

#include "include/core/SkCanvas.h"

class SkiaRenderInterface : public Rml::RenderInterface
{
public:
  SkiaRenderInterface(SkCanvas* canvas);

  SkCanvas* getCanvas();

  // Sets up OpenGL states for taking rendering commands from RmlUi.
  void BeginFrame();
  void EndFrame();

  // -- Inherited from Rml::RenderInterface --

  Rml::CompiledGeometryHandle CompileGeometry(
      Rml::Span<const Rml::Vertex> vertices,
      Rml::Span<const int> indices) override;
  void RenderGeometry(
      Rml::CompiledGeometryHandle handle,
      Rml::Vector2f translation,
      Rml::TextureHandle texture) override;
  void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

  Rml::TextureHandle LoadTexture(
      Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
  Rml::TextureHandle GenerateTexture(
      Rml::Span<const Rml::byte> source,
      Rml::Vector2i source_dimensions) override;
  void ReleaseTexture(Rml::TextureHandle texture_handle) override;

  void EnableScissorRegion(bool enable) override;
  void SetScissorRegion(Rml::Rectanglei region) override;

private:
  struct GeometryView
  {
    Rml::Span<const Rml::Vertex> vertices;
    Rml::Span<const int> indices;
  };

  SkCanvas* canvas_;
  SkRect rect_scissor = {};
  bool scissor_region_enabled = false;
};

#endif  // SKIARMLBACKEND_SKIARENDERINTERFACE_H
