#pragma once

#include "glm.h"
#include "glm_additional.h"
#include "opengl.h"

#include <memory>
#include <string>
#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;
namespace cg2
{
	enum class TextureFilter : unsigned char { None, Bilinear, Trilinear };
	enum class TextureWrap : unsigned char { BorderColor, Clamp, Repeat };

	class Texture2D
	{

	public:

		Texture2D(Texture2D const& other) = delete;
		Texture2D& operator = (Texture2D const& other) = delete;

		static std::shared_ptr<Texture2D> import(fs::path path, const TextureFilter filter = TextureFilter::Bilinear, const TextureWrap wrap = TextureWrap::BorderColor, const glm::vec4 border = glm::vec4(0.0f));

		unsigned	mHandle = 0;
		unsigned	mWidth = 0;
		unsigned	mHeight = 0;
		glm::vec4	mBorder;

	private:

		Texture2D() = default;
		~Texture2D() = default;

		struct Deleter
		{
			void operator()(Texture2D *& p) const;
		};

	};
}