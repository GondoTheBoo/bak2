#pragma once

#include "glm.h"
#include "assimp.h"

namespace toGlm
{
	inline glm::mat4 mat4( aiMatrix4x4 const& src )
	{
		return glm::mat4( src.a1, src.b1, src.c1, src.d1,
			src.a2, src.b2, src.c2, src.d2,
			src.a3, src.b3, src.c3, src.d3,
			src.a4, src.b4, src.c4, src.d4 );
	}

	inline glm::mat3 mat3( aiMatrix3x3 const& src )
	{
		return glm::mat3( src.a1, src.b1, src.c1,
			src.a2, src.b2, src.c2,
			src.a3, src.b3, src.c3 );
	}

	inline glm::vec3 vec3( aiVector3D const& src )
	{
		return glm::vec3( src.x, src.y, src.z );
	}

	inline glm::vec3 vec3( aiColor4D const& src )
	{
		return glm::vec3( src.r, src.g, src.b );
	}

	inline glm::vec2 vec2( aiVector3D const& src )
	{
		return glm::vec2( src.x, src.y );
	}
}
