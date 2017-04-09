#pragma once

#include "glm.h"
#include "glm_additional.h"
#include "opengl.h"
#include "texture2D.h"

#include <array>
#include <memory>
#include <string>
#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;
namespace cg2
{
	class TextureCube
	{

	public:

		TextureCube(TextureCube const& other) = delete;
		TextureCube& operator = (TextureCube const& other) = delete;

		static std::shared_ptr<TextureCube> import(std::array<fs::path, 6> path, const TextureFilter filter = TextureFilter::Bilinear, const TextureWrap wrap = TextureWrap::BorderColor, const glm::vec4 border = glm::vec4(0.0f));

		unsigned	mHandle = 0;
		glm::vec4	mBorder;

	private:

		TextureCube() = default;
		~TextureCube() = default;

		struct Deleter
		{
			void operator()(TextureCube *& p) const;
		};

	};
}