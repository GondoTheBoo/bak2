#include "textureCube.h"

#include "sfml.h"

#include <iostream>

namespace cg2
{
	std::shared_ptr<TextureCube> TextureCube::import(std::array< fs::path, 6 > files, const TextureFilter filter, const TextureWrap wrap, const glm::vec4 border)
	{
		std::shared_ptr<TextureCube> result(new TextureCube, TextureCube::Deleter());

		try
		{
			glGenTextures(1, &result->mHandle);
			glBindTexture(GL_TEXTURE_CUBE_MAP, result->mHandle);

			sf::Image imgs[6];
			for (unsigned int i = 0; i<6; ++i)
			{
				if (!imgs[i].loadFromFile(files[i].string()))
				{
					std::cout << "could not load " << files[i] << std::endl;
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}
				else
				{
					std::cout << "successfully loaded " << files[i] << std::endl;
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, imgs[i].getSize().x, imgs[i].getSize().y, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, imgs[i].getPixelsPtr());
				}
			}

			if (filter == TextureFilter::Bilinear)
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else if (filter == TextureFilter::Trilinear)
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			if (wrap == TextureWrap::Repeat)
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
			}
			else if (wrap == TextureWrap::Clamp)
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &border[0]);
				result->mBorder = border;
			}

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		catch (std::exception &e)
		{
			std::cout << "SFML import error " << e.what() << std::endl;
		}

		return result;
	}

	void TextureCube::Deleter::operator()(TextureCube *& p) const
	{
		if (p == nullptr)
			return;

		glDeleteTextures(1, std::addressof(p->mHandle));

		delete p;
		p = nullptr;
	}
}
