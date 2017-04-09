#include "settings.h"

#include <fstream>
#include <sstream>

namespace cg2
{
	LightType toLightType(std::string type)
	{
		if (type == "directional")
			return LightType::Directional;
		else if (type == "spot")
			return LightType::Spot;
		else if (type == "point")
			return LightType::Point;
		else
		{
			std::cout << "could not determing type of light source, defaulting to point light " << type << std::endl;
			return LightType::Point;
		}
	}

	std::string getString(const std::string key, picojson::value::object const& obj, const std::string default = "")
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				std::string val = obj.at(key).get< std::string >();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	bool getBool(const std::string key, picojson::value::object const& obj, const bool default = false)
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				bool val = obj.at(key).get< bool >();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	unsigned getUint(const std::string key, picojson::value::object const& obj, const unsigned default = 0)
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				double val = obj.at(key).get< double >();
				return static_cast<unsigned>(val);
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	float getFloat(const std::string key, picojson::value::object const& obj, const float default = 0.f)
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				double val = obj.at(key).get< double >();
				return static_cast<float>(val);
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	picojson::array getArray(const std::string key, picojson::value::object const& obj, const picojson::array default = {})
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				picojson::array val = obj.at(key).get< picojson::array >();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	picojson::object getObject(const std::string key, picojson::value::object const& obj, const picojson::object default = {})
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				picojson::object val = obj.at(key).get< picojson::object >();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	glm::vec3 getVec3(const std::string key, picojson::value::object const& obj, const glm::vec3 default = { 0.f, 0.f, 0.f })
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				glm::vec3 val;
				picojson::array arr = obj.at(key).get< picojson::array >();
				for (unsigned i = 0; i < 3; ++i)
					val[i] = (float)arr.at(i).get<double>();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	glm::vec4 getVec4(const std::string key, picojson::value::object const& obj, const glm::vec4 default = { 0.f, 0.f, 0.f, 0.f })
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				glm::vec4 val;
				picojson::array arr = obj.at(key).get< picojson::array >();
				for (unsigned i = 0; i < 4; ++i)
					val[i] = (float)arr.at(i).get<double>();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	glm::fquat getQuaternion(const std::string key, picojson::value::object const& obj, const glm::fquat default = { 0.f, 0.f, 0.f, 1.f })
	{
		try
		{
			if (obj.find(key) != obj.end())
			{
				glm::fquat val;
				picojson::array arr = obj.at(key).get< picojson::array >();
				for (unsigned i = 0; i < 4; ++i)
					val[i] = (float)arr.at(i).get<double>();
				return val;
			}
			else
			{
				std::cerr << "warning: the field " << key << " does not exist" << std::endl;
				return default;
			}
		}
		catch (...)
		{
			std::cerr << "warning: the field " << key << " exists, but has the wrong datatype" << std::endl;
		}

		return default;
	}

	std::shared_ptr< const Settings > Settings::load(const fs::path filepath)
	{
		std::shared_ptr< Settings > result(new Settings, Settings::Deleter());

		std::string settings;

		std::ifstream file(filepath.c_str(), std::ifstream::in);
		if (!file.is_open())
		{
			std::cout << "could not open settings file: " << filepath << ", are you sure the file exists?" << std::endl;
			return result;
		}

		picojson::value v;
		file >> v;
		std::string error = picojson::get_last_error();
		if (!error.empty())
		{
			std::cerr << error << std::endl;
			return result;
		}

		if (!v.is<picojson::object>())
		{
			std::cerr << "JSON is not valid" << std::endl;
			return result;
		}

		picojson::value::object const& obj = v.get<picojson::object>();

		result->mGeneral.mWindowWidth = getUint( "window width", obj, 768 );
		result->mGeneral.mWindowHeight = getUint( "window height", obj, 668 );

		auto camera = getObject( "camera", obj );
		result->mCamera.mFoV = getFloat( "field of view", camera, 30.f );
		result->mCamera.mZNear = getFloat( "near plane", camera, 0.1f );
		result->mCamera.mZFar = getFloat( "far plane", camera, 100.f );
		result->mCamera.mPosition = getVec3( "position", camera, glm::vec3( 0.f, 0.f, 5.f ) );
		result->mCamera.mCenterOfInterest = getVec3( "look at", camera, glm::vec3( 0.f, 0.f, 0.f ) );
		result->mCamera.mUpDirection = getVec3( "up direction", camera, glm::vec3( 0.f, 1.f, 0.f ) );
		//result->mCamera.mClearColor = getVec3( "background color", camera, glm::vec3( 0.f, 0.f, 0.f ) );
		
		auto environment = getObject( "environment", obj );
		result->mEnvironment.mAmbientColor = getVec3( "ambient color", environment, glm::vec3( 1.f, 1.f, 1.f ) );
		result->mEnvironment.mAmbientIntensity = getFloat( "ambient intensity", environment, 0.1f );
		result->mEnvironment.mEnvironmentMap = getString("envmap", environment );

		auto assets = getArray( "assets", obj );
		for ( unsigned i = 0; i < assets.size(); ++i )
		{
			Asset asset;
			auto assetVal = assets.at(i).get<picojson::object>();
			asset.mPath = getString( "file", assetVal );
			asset.mScaling = getVec3( "scaling", assetVal, glm::vec3( 1.f ) );
			asset.mTranslation = getVec3( "translation", assetVal, glm::vec3( 0.f ) );
			asset.mOrientation = getQuaternion( "rotation", assetVal, glm::fquat( 0.f, 0.f, 0.f, 1.f ) );
			result->mAssets.push_back(asset);
		}

		auto lights = getArray( "lights", obj );
		for ( unsigned i = 0; i < lights.size(); ++i )
		{
			LightSource light;
			auto lightVal = lights.at(i).get<picojson::object>();
			light.mType = toLightType( getString( "type", lightVal ) );
			light.mIntensity = getFloat( "intensity", lightVal, 1.f );
			light.mColor = getVec3( "color", lightVal, glm::vec3( 1.f ) );
			switch (light.mType)
			{
			case LightType::Point:
				light.mPosition = getVec3( "position", lightVal, glm::vec3( 0.f, 5.f, 0.f ) );
				break;
			case LightType::Directional:
				light.mDirection = getVec3( "direction", lightVal, glm::vec3( 0.f, -1.f, 0.f ) );
				break;
			case LightType::Spot:
				light.mPosition = getVec3( "position", lightVal, glm::vec3( 0.f, 5.f, 0.f ) );
				light.mDirection = getVec3( "direction", lightVal, glm::vec3( 0.f, -1.f, 0.f ) );
				light.mAngle = getFloat( "angle", lightVal, 30.f );
				break;
			}

			result->mLights.push_back(light);
		}

		return result;
	}

	void Settings::Deleter::operator()(Settings *& p) const
	{
		if (p == nullptr)
			return;

		delete p;
		p = nullptr;
	}
}