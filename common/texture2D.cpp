#include "texture2D.h"

#include "sfml.h"

#include <iostream>

namespace cg2
{
	std::shared_ptr<Texture2D> Texture2D::import(fs::path path, const TextureFilter filter, const TextureWrap wrap, const glm::vec4 border)
	{
		std::shared_ptr<Texture2D> result(new Texture2D, Texture2D::Deleter());

		try
		{
			sf::Image img;
			if (!img.loadFromFile(path.string()))
			{
				std::cout << "SFML import error: could not open image " << path << std::endl;
			}
			else
			{
				img.flipVertically();
				glGenTextures(1, &(result->mHandle));
				glBindTexture(GL_TEXTURE_2D, result->mHandle);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
				if (filter == TextureFilter::Bilinear)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				else if (filter == TextureFilter::Trilinear)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}

				if (wrap == TextureWrap::Repeat)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}
				else if (wrap == TextureWrap::Clamp)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border[0]);
					result->mBorder = border;
				}

				glBindTexture(GL_TEXTURE_2D, 0);

				result->mWidth = img.getSize().x;
				result->mHeight = img.getSize().y;
			}
		}
		catch (std::exception &e)
		{
			std::cout << "SFML import error " << e.what() << std::endl;
		}

		return result;
	}

	void Texture2D::Deleter::operator()(Texture2D *& p) const
	{
		if (p == nullptr)
			return;

		glDeleteTextures(1, std::addressof(p->mHandle));

		delete p;
		p = nullptr;
	}
}
