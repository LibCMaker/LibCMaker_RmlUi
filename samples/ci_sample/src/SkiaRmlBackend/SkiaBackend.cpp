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


#include "RmlUiFontEngineDefault/FontEngineInterfaceDefault.h"
#include "SkiaBackend.h"
#include "SkiaRenderInterface.h"
#include "SkiaSystemInterface.h"

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Log.h>

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

/**
    Global data used by this backend.

    Lifetime governed by the calls to Backend::Initialize() and
    Backend::Shutdown().
 */
struct SkiaBackendData
{
  SkiaBackendData(sk_sp<SkSurface> surface, SkCanvas* canvas)
      : surface_ {std::move(surface)}
      , canvas_ {canvas}
      , render_interface {canvas}
  {
  }

  sk_sp<SkSurface> surface_;
  SkCanvas* canvas_ = nullptr;
  bool running = true;

  SkiaSystemInterface system_interface;
  Rml::FontEngineInterfaceDefault font_engine_interface;
  SkiaRenderInterface render_interface;
};

static Rml::UniquePtr<SkiaBackendData> data;

bool SkiaBackend::Initialize(
    const char* /*window_name*/, int width, int height, bool /*allow_resize*/)
{
  RMLUI_ASSERT(!data);

  SkColorType colorType = SkColorType::kRGBA_8888_SkColorType;
  SkAlphaType alphaType = SkAlphaType::kOpaque_SkAlphaType;

  SkImageInfo imageInfo =
      SkImageInfo::Make(width, height, colorType, alphaType);

  sk_sp<SkSurface> surface = SkSurface::MakeRaster(imageInfo);
  if(!surface) {
    return false;
  }

  SkCanvas* canvas = surface->getCanvas();
  if(!canvas) {
    return false;
  }

  data = Rml::MakeUnique<SkiaBackendData>(surface, canvas);
  return true;
}

void SkiaBackend::Shutdown()
{
  RMLUI_ASSERT(data);
  data.reset();
}

Rml::SystemInterface* SkiaBackend::GetSystemInterface()
{
  RMLUI_ASSERT(data);
  return &data->system_interface;
}

Rml::FontEngineInterface* SkiaBackend::GetFontEngineInterface()
{
  RMLUI_ASSERT(data);
  return &data->font_engine_interface;
}

Rml::RenderInterface* SkiaBackend::GetRenderInterface()
{
  RMLUI_ASSERT(data);
  return &data->render_interface;
}

/*
bool SkiaBackend::ProcessEvents(Rml::Context* context, KeyDownCallback key_down_callback, bool power_save)
{
	RMLUI_ASSERT(data && context);

#if SDL_MAJOR_VERSION >= 3
	auto GetKey = [](const SDL_Event& event) { return event.key.key; };
	auto GetDisplayScale = []() { return SDL_GetWindowDisplayScale(data->window); };
	constexpr auto event_quit = SDL_EVENT_QUIT;
	constexpr auto event_key_down = SDL_EVENT_KEY_DOWN;
	bool has_event = false;
#else
	auto GetKey = [](const SDL_Event& event) { return event.key.keysym.sym; };
	auto GetDisplayScale = []() { return 1.f; };
	constexpr auto event_quit = SDL_QUIT;
	constexpr auto event_key_down = SDL_KEYDOWN;
	int has_event = 0;
#endif

	bool result = data->running;
	data->running = true;

	SDL_Event ev;
	if (power_save)
		has_event = SDL_WaitEventTimeout(&ev, static_cast<int>(Rml::Math::Min(context->GetNextUpdateDelay(), 10.0) * 1000));
	else
		has_event = SDL_PollEvent(&ev);

	while (has_event)
	{
		bool propagate_event = true;
		switch (ev.type)
		{
		case event_quit:
		{
			propagate_event = false;
			result = false;
		}
		break;
		case event_key_down:
		{
			propagate_event = false;
			const Rml::Input::KeyIdentifier key = RmlSDL::ConvertKey(GetKey(ev));
			const int key_modifier = RmlSDL::GetKeyModifierState();
			const float native_dp_ratio = GetDisplayScale();

			// See if we have any global shortcuts that take priority over the context.
			if (key_down_callback && !key_down_callback(context, key, key_modifier, native_dp_ratio, true))
				break;
			// Otherwise, hand the event over to the context by calling the input handler as normal.
			if (!RmlSDL::InputEventHandler(context, data->window, ev))
				break;
			// The key was not consumed by the context either, try keyboard shortcuts of lower priority.
			if (key_down_callback && !key_down_callback(context, key, key_modifier, native_dp_ratio, false))
				break;
		}
		break;
		default: break;
		}

		if (propagate_event)
			RmlSDL::InputEventHandler(context, data->window, ev);

		has_event = SDL_PollEvent(&ev);
	}

	return result;
}
*/

void SkiaBackend::RequestExit()
{
  RMLUI_ASSERT(data);
  data->running = false;
}

void SkiaBackend::BeginFrame()
{
  RMLUI_ASSERT(data);
  data->render_interface.BeginFrame();
}

void SkiaBackend::PresentFrame()
{
  RMLUI_ASSERT(data);
  data->render_interface.EndFrame();
}
