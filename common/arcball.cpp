#include "arcball.h"

namespace cg2
{
	Arcball::Arcball( const glm::uvec2 ll, const glm::uvec2 ur ) :
		mIsRotating( false ), mLowerLeft( ll ), mUpperRight( ur ), mConstraintAxis( 0.f, 1.f, 0.f ), mConstrainToAxis( false )
	{}

	void Arcball::toggleConstraintUsage()
	{
		mConstrainToAxis = !mConstrainToAxis;
	}

	void Arcball::setConstraintAxis( const glm::vec3 axis )
	{
		mConstraintAxis = axis;
	}

	glm::fquat Arcball::getCurrentRotation() const
	{ 
		return mCurrentQuat;
	}

	void Arcball::startRotation( const glm::uvec2 p )
	{
		mInitialMouse = p; mIsRotating = true;
	}

	void Arcball::endRotation()
	{
		mInitialQuat = mCurrentQuat; mIsRotating = false;
	}

	void Arcball::updateRotation( const glm::uvec2 p )
	{
		if ( !mIsRotating ) return;

		glm::vec3 va = constrainToUnitSphere( mInitialMouse, mLowerLeft, mUpperRight );
		glm::vec3 vb = constrainToUnitSphere( p, mLowerLeft, mUpperRight );

		if ( mConstrainToAxis )
		{
			va = constrainToAxis( va, mConstraintAxis );
			vb = constrainToAxis( vb, mConstraintAxis );
		}

		mCurrentQuat = mInitialQuat * glm::fquat( glm::dot( va, vb ), glm::cross( va, vb ) );
	}

	bool Arcball::isInside( const glm::uvec2 p ) const
	{
		return ( mLowerLeft.x <= p.x && mUpperRight.x >= p.x && mLowerLeft.y <= p.y && mUpperRight.y >= p.y );
	}

	glm::vec3 Arcball::constrainToUnitSphere( const glm::uvec2 pos, const glm::uvec2 ll, const glm::uvec2 ur )
	{
		glm::uvec2 dim = glm::uvec2( ur - ll );
		glm::uvec2 p = pos - ll;

		glm::vec3 pos3D = glm::vec3( 2.f*(p.x / static_cast<float>(dim.x) - 0.5f),
			2.f*((1.f - p.y / static_cast<float>(dim.y)) - 0.5f),
			0.f
			);

		float magSquared = glm::dot( pos3D, pos3D );
		if (magSquared <= 1.f)
		{
			pos3D.z = sqrt( 1.f - magSquared );
			pos3D = glm::normalize( pos3D );
		}
		else					pos3D = glm::normalize( pos3D );
		return pos3D;
	}

	glm::vec3 Arcball::constrainToAxis( const glm::vec3 loose, const glm::vec3 axis )
	{
		glm::vec3 onPlane = loose - axis * glm::dot( axis, loose );
		float magSquared = glm::dot( onPlane, onPlane );

		if (magSquared > 0.f)
		{
			if (onPlane.z < 0.f) onPlane = -onPlane;
			return (onPlane * (1.f / sqrt( magSquared )));
		}

		if (glm::dot( axis, glm::vec3( 0.f, 1.f, 0.f ) ) < 0.0001f)	onPlane = glm::vec3( 1.f, 0.f, 0.f );
		else														onPlane = glm::normalize( glm::vec3( -axis.y, axis.x, 0.0f ) );

		return onPlane;
	}
}

//#define GLM_FORCE_RADIANS