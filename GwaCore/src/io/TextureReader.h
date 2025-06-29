#pragma once

#include <ecs/components/RenderObjects.h>

namespace gwa
{
	class TextureReader
	{
	public:
		static Texture loadTexture(const char* path);
	};
}
