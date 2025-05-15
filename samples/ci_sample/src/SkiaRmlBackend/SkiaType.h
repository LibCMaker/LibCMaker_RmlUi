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


#ifndef SKIARMLBACKEND_SKIATYPE_H
#define SKIARMLBACKEND_SKIATYPE_H

#include "RmlUiFontEngineDefault/FontTypes.h"

#include "RmlUi/Core/FontMetrics.h"

namespace SkiaType
{

// Initialize SkiaType.
bool Initialise();
// Shutdown SkiaType.
void Shutdown();

// Returns a sorted list of available font variations for the font face located
// in memory.
bool GetFaceVariations(
    Rml::Span<const Rml::byte> data,
    Rml::Vector<Rml::FaceVariation>& out_face_variations,
    int face_index);

// Loads a SkiaType face from memory, 'source' is only used for logging.
Rml::SkiaTypeHandle LoadFace(
    Rml::Span<const Rml::byte> data,
    const Rml::String& source,
    int face_index,
    int named_instance_index = 0);

// Releases the SkiaType face.
bool ReleaseFace(Rml::SkiaTypeHandle face);

// Retrieves the font family, style and weight of the given font face. Use
// nullptr to ignore a property.
void GetFaceStyle(
    Rml::SkiaTypeHandle face,
    Rml::String* font_family,
    Rml::Style::FontStyle* style,
    Rml::Style::FontWeight* weight);

// Initializes a face for a given font size. Glyphs are filled with the ASCII
// subset, and the font face metrics are set.
bool InitialiseFaceHandle(
    Rml::SkiaTypeHandle face,
    int font_size,
    Rml::FontGlyphMap& glyphs,
    Rml::FontMetrics& metrics,
    bool load_default_glyphs);

// Build a new glyph representing the given code point and append to 'glyphs'.
bool AppendGlyph(
    Rml::SkiaTypeHandle face,
    int font_size,
    Rml::Character character,
    Rml::FontGlyphMap& glyphs);

// Returns the kerning between two characters.
// 'font_size' value of zero assumes the font size is already set on the face,
// and skips this step for performance reasons.
int GetKerning(
    Rml::SkiaTypeHandle face,
    int font_size,
    Rml::Character lhs,
    Rml::Character rhs);

// Returns true if the font face has kerning.
bool HasKerning(Rml::SkiaTypeHandle face);

}  // namespace SkiaType

#endif  // SKIARMLBACKEND_SKIATYPE_H
