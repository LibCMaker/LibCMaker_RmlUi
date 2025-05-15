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


#include "SkiaType.h"

#include "RmlUi/Core/ComputedValues.h"
#include "RmlUi/Core/FontMetrics.h"
#include "RmlUi/Core/Log.h"

#include <include/core/SkCanvas.h>
#include <include/core/SkData.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkSurface.h>
#include <include/core/SkTypeface.h>

#include <algorithm>

static constexpr SkColorType COLOR_TYPE = SkColorType::kGray_8_SkColorType;
static constexpr SkAlphaType ALPHA_TYPE = SkAlphaType::kOpaque_SkAlphaType;
static constexpr SkColor BACKGROUND_COLOR = SK_ColorBLACK;
static constexpr SkColor FONT_COLOR = SK_ColorWHITE;
static constexpr Rml::ColorFormat GLYPH_COLOR_FORMAT =
    (COLOR_TYPE == SkColorType::kGray_8_SkColorType) ? Rml::ColorFormat::A8
                                                     : Rml::ColorFormat::RGBA8;

static void BuildGlyphMap(
    const SkTypeface& skFace,
    const SkFont& skFont,
    const int size,
    Rml::FontGlyphMap& glyphs,
    const bool load_default_glyphs);

static bool BuildGlyphs(
    const SkTypeface& skFace,
    const SkFont& skFont,
    const SkUnichar code_cnt,
    const SkUnichar skUnichars[],
    Rml::FontGlyphMap& glyphs);

static void GenerateMetrics(const SkFont& skFont, Rml::FontMetrics& metrics);

// static int ConvertFixed16_16ToInt(int32_t fx);

bool SkiaType::Initialise()
{
  return true;
}

void SkiaType::Shutdown() {}

bool SkiaType::GetFaceVariations(
    Rml::Span<const Rml::byte> data,
    Rml::Vector<Rml::FaceVariation>& out_face_variations,
    int face_index)
{
  sk_sp<SkTypeface> typeface;

  if(sk_sp<SkData> skData = SkData::MakeWithoutCopy(data.data(), data.size())) {
    typeface =
        SkFontMgr::RefDefault()->makeFromData(std::move(skData), face_index);
  }

  if(!typeface) {
    return false;
  }

  int coordinateCount = typeface->getVariationDesignPosition(nullptr, 0);
  if(coordinateCount == -1) {
    return false;
  }

  SkFontArguments::VariationPosition::Coordinate coordinates[coordinateCount];

  coordinateCount =
      typeface->getVariationDesignPosition(coordinates, coordinateCount);
  if(coordinateCount == -1) {
    return false;
  }

  unsigned int axis_index_weight = 0;
  unsigned int axis_index_width = 1;

  const unsigned int num_axis = coordinateCount;
  for(unsigned int i = 0; i < num_axis; i++) {
    switch(coordinates[i].axis) {
      case 0x77676874:  // 'wght'
        axis_index_weight = i;
        break;
      case 0x77647468:  // 'wdth'
        axis_index_width = i;
        break;
    }
  }

  if(num_axis > 0) {
    for(unsigned int i = 0; i < coordinateCount; i++) {
      // uint16_t weight = (axis_index_weight < num_axis ?
      //     (uint16_t)ConvertFixed16_16ToInt(coordinates[axis_index_weight].value) : 0);
      // uint16_t width = (axis_index_width < num_axis ?
      //     (uint16_t)ConvertFixed16_16ToInt(coordinates[axis_index_width].value) : 0);
      uint16_t weight =
          (axis_index_weight < num_axis
               ? (uint16_t) (coordinates[axis_index_weight].value)
               : 0);
      uint16_t width =
          (axis_index_width < num_axis
               ? (uint16_t) (coordinates[axis_index_width].value)
               : 0);
      int named_instance_index = (i + 1);

      out_face_variations.push_back(Rml::FaceVariation {
          weight == 0 ? Rml::Style::FontWeight::Normal
                      : (Rml::Style::FontWeight) weight,
          width == 0 ? (uint16_t) 100 : width, named_instance_index});
    }
  }

  std::sort(out_face_variations.begin(), out_face_variations.end());

  return true;
}

Rml::SkiaTypeHandle SkiaType::LoadFace(
    Rml::Span<const Rml::byte> data,
    const Rml::String& source,
    int face_index,
    int /*named_style_index*/)
{
  if(sk_sp<SkData> skData = SkData::MakeWithoutCopy(data.data(), data.size())) {
    if(sk_sp<SkTypeface> typeface = SkFontMgr::RefDefault()->makeFromData(
           std::move(skData), face_index)) {
      return reinterpret_cast<Rml::SkiaTypeHandle>(typeface.release());
    }
  }

  Rml::Log::Message(
      Rml::Log::LT_ERROR, "Skia can not load typeface from %s.",
      source.c_str());
  return 0;

  // FT_Face face = nullptr;
  // FT_Error error = FT_New_Memory_Face(ft_library,
  //     static_cast<const FT_Byte*>(data.data()),
  //     static_cast<FT_Long>(data.size()),
  //     (named_style_index << 16) | face_index, &face);

  // // Initialise the character mapping on the face.
  // if (face->charmap == nullptr)
  // {
  //   FT_Select_Charmap(face, FT_ENCODING_APPLE_ROMAN);
  // }

  // return (SkiaTypeHandle)face;
}

bool SkiaType::ReleaseFace(Rml::SkiaTypeHandle face)
{
  if(SkTypeface* skFace = reinterpret_cast<SkTypeface*>(face)) {
    skFace->unref();
  }
  return true;
}

void SkiaType::GetFaceStyle(
    Rml::SkiaTypeHandle face,
    Rml::String* font_family,
    Rml::Style::FontStyle* style,
    Rml::Style::FontWeight* weight)
{
  SkTypeface* skFace = reinterpret_cast<SkTypeface*>(face);

  if(font_family) {
    SkString name;
    skFace->getFamilyName(&name);
    *font_family = name.c_str();
  }

  if(style) {
    *style = skFace->isItalic() ? Rml::Style::FontStyle::Italic
                                : Rml::Style::FontStyle::Normal;
  }

  if(weight) {
    SkFontStyle skFontStyle = skFace->fontStyle();
    // *weight = skFace->isBold() ? Style::FontWeight::Bold :
    // Style::FontWeight::Normal;
    *weight = static_cast<Rml::Style::FontWeight>(skFontStyle.weight());

    // TT_OS2* font_table = (TT_OS2*)FT_Get_Sfnt_Table(face, FT_SFNT_OS2);
    // if (font_table && font_table->usWeightClass != 0)
    //   *weight = (Style::FontWeight)font_table->usWeightClass;
    // else
    //   *weight = (face->style_flags & FT_STYLE_FLAG_BOLD) ==
    //   FT_STYLE_FLAG_BOLD ? Style::FontWeight::Bold :
    //   Style::FontWeight::Normal;
  }
}

bool SkiaType::InitialiseFaceHandle(
    Rml::SkiaTypeHandle face,
    int font_size,
    Rml::FontGlyphMap& glyphs,
    Rml::FontMetrics& metrics,
    bool load_default_glyphs)
{
  SkTypeface* skFace = reinterpret_cast<SkTypeface*>(face);
  RMLUI_ASSERT(skFace);
  skFace->ref();
  sk_sp<SkTypeface> spFace {skFace};

  metrics.size = font_size;

  SkFont skFont(spFace, font_size);

  // Construct the initial list of glyphs.
  BuildGlyphMap(*spFace, skFont, font_size, glyphs, load_default_glyphs);

  // Generate the metrics for the handle.
  GenerateMetrics(skFont, metrics);

  return true;
}

bool SkiaType::AppendGlyph(
    Rml::SkiaTypeHandle face,
    int font_size,
    Rml::Character character,
    Rml::FontGlyphMap& glyphs)
{
  RMLUI_ASSERT(glyphs.find(character) == glyphs.end());

  SkTypeface* skFace = reinterpret_cast<SkTypeface*>(face);
  RMLUI_ASSERT(skFace);
  skFace->ref();
  sk_sp<SkTypeface> spFace {skFace};

  SkFont skFont(spFace, font_size);

  const SkUnichar code_cnt = 1;
  SkUnichar skUnichars[code_cnt] = {static_cast<SkUnichar>(character)};
  return BuildGlyphs(*skFace, skFont, code_cnt, skUnichars, glyphs);
}

int SkiaType::GetKerning(
    Rml::SkiaTypeHandle face,
    int /*font_size*/,
    Rml::Character lhs,
    Rml::Character rhs)
{
  // TODO: SkTypeface::getKerningPairAdjustments() works only with FreeType
  // backend. Use FreeType for all OSs.

  SkTypeface* skFace = reinterpret_cast<SkTypeface*>(face);
  SkGlyphID glyphs[] = {
      static_cast<SkGlyphID>(lhs), static_cast<SkGlyphID>(rhs)};
  int32_t adjustments[1];
  bool res = skFace->getKerningPairAdjustments(glyphs, 2, adjustments);
  return res ? adjustments[0] >> 6 : 0;
}

bool SkiaType::HasKerning(Rml::SkiaTypeHandle face)
{
  SkTypeface* skFace = reinterpret_cast<SkTypeface*>(face);
  return skFace->getKerningPairAdjustments(nullptr, 0, nullptr);
}

static void BuildGlyphMap(
    const SkTypeface& skFace,
    const SkFont& skFont,
    const int size,
    Rml::FontGlyphMap& glyphs,
    const bool load_default_glyphs)
{
  if(load_default_glyphs) {
    glyphs.reserve(128);

    // Add the ASCII characters now. Other characters are added later as needed.
    const SkUnichar code_min = static_cast<SkUnichar>(32);
    const SkUnichar code_max = static_cast<SkUnichar>(127);
    const SkUnichar code_cnt = code_max - code_min;

    SkUnichar skUnichars[code_cnt];

    for(SkUnichar char_code = code_min; char_code < code_max; ++char_code) {
      skUnichars[char_code - code_min] = char_code;
    }

    BuildGlyphs(skFace, skFont, code_cnt, skUnichars, glyphs);
  }

  // Add a replacement character for rendering unknown characters.
  Rml::Character replacement_character = Rml::Character::Replacement;
  auto it = glyphs.find(replacement_character);

  if(it == glyphs.end()) {
    Rml::FontGlyph glyph;
    glyph.bitmap_dimensions = {size / 3, (size * 2) / 3};
    glyph.advance = glyph.bitmap_dimensions.x + 2;
    glyph.bearing = {1, glyph.bitmap_dimensions.y};

    glyph.bitmap_owned_data.reset(
        new Rml::byte[glyph.bitmap_dimensions.x * glyph.bitmap_dimensions.y]);
    glyph.bitmap_data = glyph.bitmap_owned_data.get();

    for(int y = 0; y < glyph.bitmap_dimensions.y; ++y) {
      for(int x = 0; x < glyph.bitmap_dimensions.x; ++x) {
        constexpr int stroke = 1;
        int i = y * glyph.bitmap_dimensions.x + x;
        bool near_edge =
            (x < stroke || x >= glyph.bitmap_dimensions.x - stroke || y < stroke
             || y >= glyph.bitmap_dimensions.y - stroke);
        glyph.bitmap_owned_data[i] = (near_edge ? 0xdd : 0);
      }
    }

    glyphs[replacement_character] = std::move(glyph);
  }
}

static bool BuildGlyphs(
    const SkTypeface& skFace,
    const SkFont& skFont,
    const SkUnichar code_cnt,
    const SkUnichar skUnichars[],
    Rml::FontGlyphMap& glyphs)
{
  SkGlyphID skGlyphs[code_cnt];
  SkScalar widths[code_cnt];
  SkRect bounds[code_cnt];
  // SkPoint skPos[code_cnt];

  SkPaint skPaint;
  skPaint.setColor(FONT_COLOR);

  skFace.unicharsToGlyphs(skUnichars, code_cnt, skGlyphs);
  skFont.getWidthsBounds(skGlyphs, code_cnt, widths, bounds, &skPaint);
  // skFont.getPos(skGlyphs, code_cnt, skPos);

  for(size_t i = 0; i < code_cnt; ++i) {
    auto result = glyphs.emplace(static_cast<Rml::Character>(skUnichars[i]), Rml::FontGlyph {});
    if(!result.second) {
      // Log::Message(Log::LT_WARNING, "Glyph character '%u' is already loaded in
      // the font face '%s %s'.", (unsigned int)character,
      //   ft_face->family_name, ft_face->style_name);
      return false;
    }

    Rml::FontGlyph& glyph = result.first->second;

    // // glyph.bearing.x = ft_glyph->bitmap_left;
    // glyph.bearing.x = static_cast<int>(-bounds[i].x());
    // glyph.bearing.x = static_cast<int>(skPos.x());
    glyph.bearing.x = 0;  //
    // // glyph.bearing.y = ft_glyph->bitmap_top;
    // glyph.bearing.y = static_cast<int>(skPos.y());
    glyph.bearing.y = static_cast<int>(-bounds[i].y());
    // // glyph.advance = ft_glyph->metrics.horiAdvance >> 6;
    glyph.advance = static_cast<int>(widths[i]);
    // // glyph.bitmap_dimensions.x = ft_glyph->bitmap.width;
    glyph.bitmap_dimensions.x = static_cast<int>(bounds[i].width());
    // // glyph.bitmap_dimensions.y = ft_glyph->bitmap.rows;
    glyph.bitmap_dimensions.y = static_cast<int>(bounds[i].height());

    // Draw glyph to our glyph handle.
    if(glyph.bitmap_dimensions.x > 0 && glyph.bitmap_dimensions.y > 0) {
      SkImageInfo imageInfo = SkImageInfo::Make(
          glyph.bitmap_dimensions.x, glyph.bitmap_dimensions.y, COLOR_TYPE,
          ALPHA_TYPE);

      const int num_bytes_per_pixel = imageInfo.bytesPerPixel();
      glyph.color_format = GLYPH_COLOR_FORMAT;

      glyph.bitmap_owned_data.reset(
          new Rml::byte
              [glyph.bitmap_dimensions.x * glyph.bitmap_dimensions.y
               * num_bytes_per_pixel]);
      glyph.bitmap_data = glyph.bitmap_owned_data.get();
      Rml::byte* destination_bitmap = glyph.bitmap_owned_data.get();

      sk_sp<SkSurface> skSurface = SkSurface::MakeRasterDirect(
          imageInfo, destination_bitmap, imageInfo.minRowBytes());
      if(!skSurface) {
        return false;
      }

      SkCanvas* skCanvas = skSurface->getCanvas();
      if(!skCanvas) {
        return false;
      }

      SkPoint skPositions[] = {
          {static_cast<SkScalar>(glyph.bearing.x),
           static_cast<SkScalar>(glyph.bearing.y)}};
      skCanvas->clear(BACKGROUND_COLOR);
      skCanvas->drawGlyphs(1, &skGlyphs[i], skPositions, {0, 0}, skFont, skPaint);
    }
  }

  return true;
}

static void GenerateMetrics(const SkFont& skFont, Rml::FontMetrics& metrics)
{
  SkFontMetrics skFontMetrics;
  SkScalar skLineSpacing = skFont.getMetrics(&skFontMetrics);

  // // metrics.ascent = ft_face->size->metrics.ascender / float(1 << 6);
  metrics.ascent = -skFontMetrics.fAscent;
  // // metrics.descent = -ft_face->size->metrics.descender / float(1 << 6);
  metrics.descent = skFontMetrics.fDescent;
  // // metrics.line_spacing = ft_face->size->metrics.height / float(1 << 6);
  metrics.line_spacing = skLineSpacing;

  SkScalar skUnderlinePosition;
  if(skFontMetrics.hasUnderlinePosition(&skUnderlinePosition)) {
    // metrics.underline_position = FT_MulFix(-ft_face->underline_position,
    // ft_face->size->metrics.y_scale) / float(1 << 6);
    // TODO: -skUnderlinePosition
    metrics.underline_position = skUnderlinePosition;
  }

  SkScalar skUnderlineThickness;
  if(skFontMetrics.hasUnderlineThickness(&skUnderlineThickness)) {
    // metrics.underline_thickness =
    //     FT_MulFix(ft_face->underline_thickness,
    //     ft_face->size->metrics.y_scale) * bitmap_scaling_factor / float(1 <<
    //     6);
    // metrics.underline_thickness =
    // Math::Max(metrics.underline_thickness, 1.0f);
    metrics.underline_thickness = Rml::Math::Max(skUnderlineThickness, 1.0f);
  }

  // Determine the x-height of this font face.
  if(skFontMetrics.fXHeight != 0) {
    metrics.x_height = skFontMetrics.fXHeight;
  } else {
    metrics.x_height = 0.5f * metrics.line_spacing;
  }
}

// static int ConvertFixed16_16ToInt(int32_t fx)
// {
//   return fx / 0x10000;
// }
