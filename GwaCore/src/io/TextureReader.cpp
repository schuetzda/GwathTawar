#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "TextureReader.h"
namespace gwa
{
	Texture TextureReader::loadTexture(const char* path)
	{
		int texWidth{ 0 };
		int texHeight{ 0 };
		int texChannels{ 0 };
		uint8_t* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		return Texture(texWidth, texHeight, pixels);
	}
}