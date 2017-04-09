#include "../../common/opengl.h"
#include "../../common/sfml.h"
#include "../../common/glm.h"
#include "../../common/glsl.h"
#include "../../common/arcball.h"
#include "../../common/triasset.h"
#include "../../common/texture2D.h"
#include "../../common/textureCube.h"
#include "../../common/settings.h"

#include <iostream>

enum class ShadingType : unsigned { SolidColor = 0, Environment = 1, Reflective = 2 };

int main( int argc, char** argv )
{
#pragma region Settings
	std::string settingsFile = ( ( argc < 2 ) ? "settings\\pbr_ogre.txt" : argv[1] );
	std::shared_ptr< const cg2::Settings > settings = cg2::Settings::load( settingsFile );
	if (settings == nullptr)
	{
		std::cerr << "settings file not found" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitWindowAndOpenGL
	sf::Window window( sf::VideoMode( settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight, 32 ), "abyssus abyssum invocat", sf::Style::Close, sf::ContextSettings( 16, 0, 4, 3, 3 ) );
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glew init failed" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitCameraMatrices
	const float aspectRatio = settings->mGeneral.mWindowWidth / static_cast<float>( settings->mGeneral.mWindowHeight );
	glm::mat4 viewTf = glm::lookAt( settings->mCamera.mPosition, settings->mCamera.mCenterOfInterest, settings->mCamera.mUpDirection );
	glm::mat4 projectionTf = glm::perspective( glm::radians( settings->mCamera.mFoV ), aspectRatio, settings->mCamera.mZNear, settings->mCamera.mZFar );
	glm::vec3 cameraPosition = settings->mCamera.mPosition;
#pragma endregion

#pragma region LoadGlslShaders
	std::shared_ptr< cg2::GlslProgram > glslProgram = cg2::GlslProgram::create({ cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\practical_exercises_unit6\\envmap.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\practical_exercises_unit6\\envmap.frag")) }, true );
#pragma endregion

#pragma region ImportAssets
	char * toDataEnv = std::getenv("VCG_2016_DATA_DIR");
	fs::path dataPath = (toDataEnv != nullptr ? toDataEnv : "../data");
	struct Asset
	{
		std::shared_ptr< const cg2::TriAsset >	model;
		glm::mat4								sceneTf;
	};
	std::vector< Asset > assets;
	for (auto a : settings->mAssets)
	{
		Asset asset;
		asset.model = cg2::TriAsset::import(dataPath / "models" / a.mPath);
		std::cout << a.mOrientation.x << " " << a.mOrientation.y << " " << a.mOrientation.z << " " << a.mOrientation.w;
		asset.sceneTf = glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), a.mTranslation), glm::angle(a.mOrientation), glm::axis(a.mOrientation)), a.mScaling);
		assets.push_back(asset);
	}
	std::shared_ptr< const cg2::TriAsset > environmentModel = cg2::TriAsset::import(dataPath / "models/cubes/white_cube.obj");
	std::shared_ptr< const cg2::TriAsset > pointlightModel = cg2::TriAsset::import(dataPath / "models/spheres/sphere_lowres.obj");
	std::shared_ptr< const cg2::TriAsset > spotlightModel = cg2::TriAsset::import(dataPath / "models/cones/cone_lowres.obj");
#pragma endregion

#pragma region EnvironmentMap
	std::string envBase = ( dataPath / "textures" / settings->mEnvironment.mEnvironmentMap ).string();
	std::shared_ptr< const cg2::TextureCube > environmentMap = cg2::TextureCube::import({ envBase + "/posx.jpg", envBase + "/negx.jpg", envBase + "/posy.jpg", envBase + "/negy.jpg", envBase + "/posz.jpg", envBase + "/negz.jpg" }, cg2::TextureFilter::Trilinear, cg2::TextureWrap::Clamp);
#pragma endregion

	cg2::Arcball cameraController( glm::uvec2( 0, 0 ), glm::uvec2( settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowWidth ) );
	cameraController.setConstraintAxis( glm::vec3( 0.f, 1.f, 0.f ) ); cameraController.toggleConstraintUsage();

	while ( true )
	{

#pragma region HandleUserInput
		bool exit = false;
		sf::Event ev;
		while ( window.pollEvent( ev ) )
		{
			if ( sf::Event::Closed == ev.type )
			{
				exit = true;
				break;
			}
			else if ( sf::Event::MouseButtonPressed == ev.type && ev.mouseButton.button == sf::Mouse::Left )
			{
				cameraController.startRotation( glm::uvec2( ev.mouseButton.x, ev.mouseButton.y ) );
			}
			else if ( sf::Event::MouseButtonReleased == ev.type && ev.mouseButton.button == sf::Mouse::Left )
			{
				cameraController.endRotation();
			}
			else if ( sf::Event::MouseMoved == ev.type )
			{
				cameraController.updateRotation( glm::uvec2( ev.mouseMove.x, ev.mouseMove.y ) );
			}
			else if ( sf::Event::KeyPressed == ev.type )
			{
				switch (ev.key.code)
				{
				case sf::Keyboard::Space:
					glslProgram = cg2::GlslProgram::create({ cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\pbr.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\pbr.frag")) }, true);
					break;
				default:
					break;
				}
			}
		}
		if (exit)
		{
			break;
		}
#pragma endregion

		glEnable(GL_MULTISAMPLE);

#pragma region Render
		const glm::vec4 bgColor = glm::vec4(settings->mEnvironment.mAmbientIntensity * settings->mEnvironment.mAmbientColor, 1.f);
		const float one = 1.f;
		glClearBufferfv( GL_DEPTH, 0, &one );
		glClearBufferfv( GL_COLOR, 0, &bgColor[0] );

		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );

		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );

		glViewport( 0, 0, settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight );

		cg2::GlslProgram::setActiveProgram( glslProgram );

		glslProgram->setUniformVec4( "cameraPosition", glm::vec4( cameraPosition, 1 ) );
		glslProgram->setUniformIVal( "shadingType", (int)ShadingType::Reflective );

		for ( auto asset : assets )
		{
			for ( auto node : asset.model->mScenegraph )
			{
				glm::mat4 modelTf = asset.sceneTf * node->mModelTf;
				glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
				glm::mat3 normals2worldTf = glm::mat3( glm::inverse( glm::transpose( modelTf ) ) );

				glslProgram->setUniformMat4( "modelTf", modelTf );
				glslProgram->setUniformMat4( "mvpTf", mvpTf );
				glslProgram->setUniformMat3( "normals2worldTf", normals2worldTf );

				for ( auto id : node->mMeshIds )
				{
					auto meshData = asset.model->mMeshDataCPU[id];
					auto vao = asset.model->mVAOs[id];

					if (!meshData.mMaterial.mAOTexture.empty())
					{
						auto texture = asset.model->mTextures.at(meshData.mMaterial.mAOTexture);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, texture->mHandle);
						glslProgram->setUniformBVal("hasAoTexture", true);
					}
					else
					{
						glslProgram->setUniformBVal("hasAoTexture", false);
					}

					glslProgram->setUniformTexVal("aoTexture", 1);

					if (!meshData.mMaterial.mNormalMap.empty())
					{
						auto texture = asset.model->mTextures.at(meshData.mMaterial.mNormalMap);
						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, texture->mHandle);
						glslProgram->setUniformBVal("hasNormalTexture", true);
					}
					else
					{
						glslProgram->setUniformBVal("hasNormalTexture", false);
					}

					glslProgram->setUniformTexVal("normalTexture",2);

					glBindVertexArray( vao );
					glDrawElements( GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr );
				}
			}
		}

		glslProgram->setUniformIVal( "shadingType", (int)ShadingType::Environment  );

		for ( auto node : environmentModel->mScenegraph )
		{
			glm::mat4 modelTf = glm::scale( glm::mat4( 1.f ), glm::vec3( 10.f ) ) * node->mModelTf;
			glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
			glslProgram->setUniformMat4( "mvpTf", mvpTf );
			glslProgram->setUniformMat4( "modelTf", modelTf );

			for ( auto id : node->mMeshIds )
			{
				auto meshData = environmentModel->mMeshDataCPU[id];
				auto vao = environmentModel->mVAOs[id];
				glBindVertexArray( vao );
				glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
			}
		}

		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, 0 );

		glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );
#pragma endregion

#pragma region Update
		glm::mat3 cameraRotation = glm::mat3_cast(cameraController.getCurrentRotation());
		glm::vec3 cameraViewDirection = settings->mCamera.mPosition - settings->mCamera.mCenterOfInterest;
		cameraPosition = settings->mCamera.mCenterOfInterest + cameraRotation * cameraViewDirection;
		glm::vec3 cameraUpDirection = cameraRotation * settings->mCamera.mUpDirection;
		viewTf = glm::lookAt(cameraPosition, settings->mCamera.mCenterOfInterest, cameraUpDirection);
#pragma endregion

		window.display();
	}

	return 0;
}