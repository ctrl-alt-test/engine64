#pragma once

#include "engine/debug/DebugSettings.hpp"
#include "engine/EngineConfig.hpp"

namespace Core
{
	class Settings
	{
	public:
#if ENABLE_QUALITY_OPTION
		enum GraphicsQuality : int {
#if ENABLE_EDITING
			RidiculouslyLow,
			VeryLow,
#endif // ENABLE_EDITING
			Low,
			Medium,
			High,

			MaxQuality = High,
		};
#endif // ENABLE_QUALITY_OPTION

		Settings():
			renderWidth(1280),
			renderHeight(720),
			windowWidth(1280),
			windowHeight(720),
			fullscreen(true)
#if ENABLE_QUALITY_OPTION
			,
			m_textureSizeMultiplier(2),
			m_textureSizeDivider(1)
#endif // ENABLE_QUALITY_OPTION
		{
#if ENABLE_EDITING
			fullscreen = false;
			SetQuality(Core::Settings::RidiculouslyLow);
#endif // ENABLE_EDITING
		}

#if ENABLE_QUALITY_OPTION
		void SetQuality(GraphicsQuality quality);
		int TextureSizeAbout(int size) const;
#else // !ENABLE_QUALITY_OPTION
		int TextureSizeAbout(int size) const;
#endif // !ENABLE_QUALITY_OPTION

		int TextureSizeAtMost(int size) const;
		int TextureSizeAtLeast(int size) const;

		int				renderWidth;
		int				renderHeight;

		int				windowWidth;
		int				windowHeight;

		bool			fullscreen;

#if DEBUG
		Debug::DebugSettings	debug;
#endif // DEBUG

#if ENABLE_QUALITY_OPTION
	private:
		int				m_textureSizeMultiplier;
		int				m_textureSizeDivider;
#endif // ENABLE_QUALITY_OPTION
	};
}
