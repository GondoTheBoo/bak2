#pragma once

#define GLM_FORCE_RADIANS

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/quaternion.hpp"

namespace cg2
{

	class Arcball
	{

	public:

		Arcball( const glm::uvec2 lowerLeft, const glm::uvec2 upperRight );

		void toggleConstraintUsage();
		void setConstraintAxis( const glm::vec3 axis );

		glm::fquat getCurrentRotation() const;

		void startRotation( const glm::uvec2 startPosition );
		void updateRotation( const glm::uvec2 currentPosition );
		void endRotation();

		bool isInside( const glm::uvec2 position ) const;

		static glm::vec3 constrainToUnitSphere( const glm::uvec2 position, const glm::uvec2 lowerLeft, const glm::uvec2 upperRight );
		static glm::vec3 constrainToAxis( const glm::vec3 loose, const glm::vec3 axis );

	private:

		glm::uvec2			mLowerLeft;
		glm::uvec2			mUpperRight;
		bool				mIsRotating;
		bool				mConstrainToAxis;
		glm::vec3			mConstraintAxis;
		glm::uvec2			mInitialMouse;
		glm::fquat			mInitialQuat;
		glm::fquat			mCurrentQuat;

	};

}