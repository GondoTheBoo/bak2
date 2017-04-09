#pragma once

#include "../common/glm.h"
#include "../common/glm_additional.h"
#include "../common/json.h"

#include <memory>
#include <string>
#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;

enum class LightType : unsigned char { Point = 0, Directional = 1, Spot = 2 };

namespace cg2
{
	class Settings
	{

	public:

		Settings( Settings const& other ) = delete;
		Settings& operator = ( Settings const& other ) = delete;

		static std::shared_ptr< const Settings > load( const fs::path path );

		struct General
		{
			unsigned			mWindowWidth = 768;
			unsigned			mWindowHeight = 768;
		};

		struct Camera
		{
			float				mFoV = 1.f;
			float				mZNear = 0.1f;
			float				mZFar = 10.f;
			glm::vec3			mPosition = { 0.f, 0.f, 5.f };
			glm::vec3			mCenterOfInterest = { 0.f, 0.f, 0.f };
			glm::vec3			mUpDirection = { 0.f, 1.f, 0.f };
			//glm::vec3			mClearColor = { 0.f, 0.f, 0.f };
		};

		struct Asset
		{
			fs::path			mPath;
			glm::vec3			mTranslation = { 0.f, 0.f, 0.f };
			glm::quat			mOrientation = { 0.f, 0.f, 0.f, 1.f };
			glm::vec3			mScaling = { 1.f, 1.f, 1.f };
		};

		struct LightSource
		{
			LightType			mType = LightType::Point;
			glm::vec3			mColor = { 1.f, 1.f, 1.f };
			float				mIntensity = 1.f;
			glm::vec3			mPosition = { 0.f, 2.f, 0.f };
			float				mAngle = 0.f;
			glm::vec3			mDirection = { 0.f, 0.f, 0.f };
		};

		struct Environment
		{
			glm::vec3			mAmbientColor = { 1.f, 1.f, 1.f };
			float				mAmbientIntensity = 0.1f;
			fs::path			mEnvironmentMap;
		};

		General						mGeneral;
		Camera						mCamera;
		Environment					mEnvironment;
		std::vector< Asset >		mAssets;
		std::vector< LightSource >	mLights;

	private:

		Settings() = default;
		~Settings() = default;

		struct Deleter
		{
			void operator()( Settings *& p ) const;
		};

	};
}