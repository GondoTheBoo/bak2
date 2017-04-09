#include "../../common/opengl.h"
#include "../../common/sfml.h"
#include "../../common/glm.h"
#include "../../common/glsl.h"
#include "../../common/arcball.h"
#include "../../common/triasset.h"
#include "../../common/texture2D.h"
#include "../../common/settings.h"

#include <iostream>

int main( int argc, char** argv )
{
#pragma region Settings
	std::string settingsFile = ( ( argc < 2 ) ? "settings/ogre_pointlights.txt" : argv[1] );
	std::shared_ptr< const cg2::Settings > settings = cg2::Settings::load( settingsFile );
	if (settings == nullptr)
	{
		std::cerr << "settings file not found" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitWindowAndOpenGL
	sf::Window window( sf::VideoMode( settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight, 32 ), "deferred singlepass", sf::Style::Close, sf::ContextSettings( 16, 0, 0, 3, 3 ) );
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glew init failed" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitCameraMatrices
	const float aspectRatio = settings->mGeneral.mWindowWidth / static_cast<float>( settings->mGeneral.mWindowHeight );
	glm::mat4 viewTf = glm::lookAt( settings->mCamera.mPosition, settings->mCamera.mCenterOfInterest, settings->mCamera.mUpDirection );
	const glm::mat4 projectionTf = glm::perspective( glm::radians( settings->mCamera.mFoV ), aspectRatio, settings->mCamera.mZNear, settings->mCamera.mZFar );
	const glm::mat4 viewportTf = glm::mat4(glm::vec4(settings->mGeneral.mWindowWidth / 2, 0.f, 0.f, 0.f), glm::vec4(0.f, settings->mGeneral.mWindowHeight / 2, 0.f, 0.f), glm::vec4(0.f, 0.f, 0.5f, 0.f), glm::vec4(settings->mGeneral.mWindowWidth / 2, settings->mGeneral.mWindowHeight / 2, 0.5f, 1.f));
	const glm::mat4 screen2EyeTf = glm::inverse(viewportTf * projectionTf);	// needed to reconstruct position from depth
#pragma endregion

#pragma region LoadGlslShaders
	std::vector< cg2::ShaderInfo > gBufferShaders = { cg2::ShaderInfo( cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource( "shaders\\solution_deferred\\buildGBuffer.vert" ) ), cg2::ShaderInfo( cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource( "shaders\\solution_deferred\\buildGBuffer.frag" ) ) };
	std::shared_ptr< cg2::GlslProgram > buildGBufferProgram = cg2::GlslProgram::create(gBufferShaders, true );
	std::vector< cg2::ShaderInfo > evalLightsShaders = { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalAllLights.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalAllLights.frag")) };
	std::shared_ptr< cg2::GlslProgram > evalLightsProgram = cg2::GlslProgram::create(evalLightsShaders, true);
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
	for ( auto a : settings->mAssets )
	{
		Asset asset;
		asset.model = cg2::TriAsset::import( dataPath / "models" / a.mPath );
		asset.sceneTf = glm::translate(glm::mat4(1.f), a.mTranslation) * glm::mat4_cast(a.mOrientation) * glm::scale(glm::mat4(1.f), a.mScaling);
		assets.push_back( asset );
	}
	GLuint fullscreenQuad = 0;
	glGenVertexArrays(1, &fullscreenQuad);
	std::shared_ptr< const cg2::TriAsset > pointlightModel = cg2::TriAsset::import(dataPath / "models/spheres/sphere_lowres.obj");
	std::shared_ptr< const cg2::TriAsset > spotlightModel = cg2::TriAsset::import(dataPath / "models/cones/cone_lowres.obj");
#pragma endregion

#pragma region CreateFBO
	const unsigned int fboWidth = settings->mGeneral.mWindowWidth;
	const unsigned int fboHeight = settings->mGeneral.mWindowHeight;
	GLuint handleToGBuffer = 0;
	std::vector< GLuint > handleToGBufferTextures(5, 0);

	const glm::vec4 normalsDefault(0.f, 0.f, 0.f, 0.f);
	const glm::vec4 diffuseDefault(settings->mEnvironment.mAmbientColor * settings->mEnvironment.mAmbientIntensity, 1.f);
	const glm::vec4 specularDefault(0.f, 0.f, 0.f, 0.f);
	const glm::vec4 ambientDefault(0.f, 0.f, 0.f, 0.f);

	for (unsigned i = 0; i < handleToGBufferTextures.size(); ++i) {
		glGenTextures(1, std::addressof(handleToGBufferTextures[i]));
		glBindTexture(GL_TEXTURE_2D, handleToGBufferTextures[i]);
		if (i==0)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	glGenFramebuffers(1, std::addressof(handleToGBuffer));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToGBuffer);

	for (unsigned i = 0; i < handleToGBufferTextures.size(); ++i) {
		if (i==0)
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, handleToGBufferTextures[i], 0);
		else
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (i-1), handleToGBufferTextures[i], 0);
	}

	std::vector< GLenum > fboBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(fboBuffers.size(), fboBuffers.data());

	GLenum statusCode = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (statusCode != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer invalid, status code " << statusCode << std::endl;
		return -1;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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
		}
		if (exit)
		{
			break;
		}
#pragma endregion

#pragma region RenderPass1
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToGBuffer);
		const float one = 1.f;
		glClearBufferfv(GL_DEPTH, 0, &one);
		glClearBufferfv(GL_COLOR, 0, &normalsDefault[0]);
		glClearBufferfv(GL_COLOR, 1, &diffuseDefault[0]);
		glClearBufferfv(GL_COLOR, 2, &specularDefault[0]);
		glClearBufferfv(GL_COLOR, 3, &ambientDefault[0]);

		glViewport( 0, 0, fboWidth, fboHeight );

		cg2::GlslProgram::setActiveProgram( buildGBufferProgram );
		buildGBufferProgram->setUniformIVal("solidFlag", 0);
		for ( auto asset : assets )
		{
			for ( auto node : asset.model->mScenegraph )
			{
				glm::mat4 modelTf = asset.sceneTf * node->mModelTf;
				glm::mat4 modelviewTf = viewTf * modelTf;
				glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
				glm::mat3 normals2eyeTf = glm::mat3( glm::inverse(glm::transpose(modelviewTf)) );

				buildGBufferProgram->setUniformMat4("mvpTf", mvpTf);
				buildGBufferProgram->setUniformMat3("normals2eyeTf", normals2eyeTf);

				for ( auto id : node->mMeshIds )
				{
					auto meshData = asset.model->mMeshDataCPU[id];
					auto vao = asset.model->mVAOs[id];

					if (!meshData.mMaterial.mDiffuseTexture.empty())
					{
						auto texture = asset.model->mTextures.at(meshData.mMaterial.mDiffuseTexture);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texture->mHandle);
						buildGBufferProgram->setUniformBVal("hasDiffuseTexture", true);
					}
					else
					{
						buildGBufferProgram->setUniformBVal("hasDiffuseTexture", false);
					}

					buildGBufferProgram->setUniformTexVal("diffuseTexture", 0);
					buildGBufferProgram->setUniformVec3("kDiffuse", meshData.mMaterial.mKDiffuse);

					if (!meshData.mMaterial.mSpecularTexture.empty())
					{
						auto texture = asset.model->mTextures.at(meshData.mMaterial.mSpecularTexture);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, texture->mHandle);
						buildGBufferProgram->setUniformBVal("hasSpecularTexture", true);
					}
					else
					{
						buildGBufferProgram->setUniformBVal("hasSpecularTexture", false);
					}

					buildGBufferProgram->setUniformTexVal("specularTexture", 1);
					buildGBufferProgram->setUniformVec3("kSpecular", meshData.mMaterial.mKSpecular);

					if (!meshData.mMaterial.mAOTexture.empty())
					{
						auto texture = asset.model->mTextures.at(meshData.mMaterial.mAOTexture);
						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, texture->mHandle);
						buildGBufferProgram->setUniformBVal("hasAmbientTexture", true);
					}
					else
					{
						buildGBufferProgram->setUniformBVal("hasAmbientTexture", false);
					}

					buildGBufferProgram->setUniformTexVal("ambientTexture", 2);
					buildGBufferProgram->setUniformVec3("kAmbient", meshData.mMaterial.mKAmbient);

					buildGBufferProgram->setUniformVal("shininess", meshData.mMaterial.mShininess);
					
					glBindVertexArray( vao );
					glDrawElements( GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr );
				}
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		buildGBufferProgram->setUniformIVal("solidFlag", 1);

		for (auto light : settings->mLights)
		{
			if (light.mType == LightType::Point)
			{
				float scale = 0.2f * light.mIntensity;
				glm::mat4 lightTf = glm::translate(glm::mat4(1.f), light.mPosition) * glm::scale(glm::mat4(1.f), glm::vec3(scale));

				for (auto node : pointlightModel->mScenegraph)
				{
					glm::mat4 modelTf = lightTf * node->mModelTf;
					glm::mat4 modelviewTf = viewTf * modelTf;
					glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
					buildGBufferProgram->setUniformMat4("mvpTf", mvpTf);
					buildGBufferProgram->setUniformBVal("hasDiffuseTexture", false);
					buildGBufferProgram->setUniformTexVal("diffuseTexture", 0);
					buildGBufferProgram->setUniformVec3("kDiffuse", light.mColor);

					for (auto id : node->mMeshIds)
					{
						auto meshData = pointlightModel->mMeshDataCPU[id];
						auto vao = pointlightModel->mVAOs[id];
						glBindVertexArray(vao);
						glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
					}
				}
			}
			else if (light.mType == LightType::Spot)
			{
				float scale = 0.2f * light.mIntensity;
				glm::mat4 lightTf = glm::translate(glm::mat4(1.f), light.mPosition) * glm::mat4_cast(glm::fquat(glm::vec3(0.f, 1.f, 0.f), glm::normalize(light.mDirection))) * glm::scale(glm::mat4(1.f), glm::vec3(scale*(1 / glm::tan(glm::radians(90 - light.mAngle))), scale, scale*(1 / glm::tan(glm::radians(90 - light.mAngle)))));

				for (auto node : spotlightModel->mScenegraph)
				{
					glm::mat4 modelTf = lightTf * node->mModelTf;
					glm::mat4 modelviewTf = viewTf * modelTf;
					glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
					buildGBufferProgram->setUniformMat4("mvpTf", mvpTf);
					buildGBufferProgram->setUniformBVal("hasDiffuseTexture", false);
					buildGBufferProgram->setUniformTexVal("diffuseTexture", 0);
					buildGBufferProgram->setUniformVec3("kDiffuse", light.mColor);

					for (auto id : node->mMeshIds)
					{
						auto meshData = spotlightModel->mMeshDataCPU[id];
						auto vao = spotlightModel->mVAOs[id];
						glBindVertexArray(vao);
						glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
					}
				}
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_DEPTH_TEST);
#pragma endregion

#pragma region RenderPass2
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight);

		cg2::GlslProgram::setActiveProgram(evalLightsProgram);
		evalLightsProgram->setUniformVal( "ambientIntensity", settings->mEnvironment.mAmbientIntensity );
		evalLightsProgram->setUniformVec3( "ambientColor", settings->mEnvironment.mAmbientColor );

		evalLightsProgram->setUniformIVal("lightCount", settings->mLights.size());
		for (unsigned i = 0; i < settings->mLights.size(); ++i)
		{
			std::string base = "lightSources[" + std::to_string(i) + "]";
			auto light = settings->mLights[i];

			evalLightsProgram->setUniformUVal(base+".type", (unsigned)light.mType);
			evalLightsProgram->setUniformVal(base+".intensity", light.mIntensity);
			evalLightsProgram->setUniformVec3(base+".color", light.mColor);

			if (light.mType == LightType::Directional)
			{
				glm::vec4 lightDirection = viewTf * glm::vec4(light.mDirection, 0.f);
				evalLightsProgram->setUniformVec3(base+".direction", glm::vec3(lightDirection));
			}
			else if (light.mType == LightType::Point)
			{
				glm::vec4 lightPosition = viewTf * glm::vec4(light.mPosition, 1.f);
				evalLightsProgram->setUniformVec4(base+".position", lightPosition);
			}
			else
			{
				glm::vec4 lightDirection = viewTf * glm::vec4(light.mDirection, 0.f);
				evalLightsProgram->setUniformVec3(base+".direction", glm::vec3(lightDirection));
				glm::vec4 lightPosition = viewTf * glm::vec4(light.mPosition, 1.f);
				evalLightsProgram->setUniformVec4(base+".position", lightPosition);
				evalLightsProgram->setUniformVal(base+".cosangle", glm::cos(glm::radians(light.mAngle)));
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, handleToGBufferTextures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, handleToGBufferTextures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, handleToGBufferTextures[2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, handleToGBufferTextures[3]);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, handleToGBufferTextures[4]);

		cg2::GlslProgram::setActiveProgram(evalLightsProgram);
		evalLightsProgram->setUniformTexVal("textureA", 0);
		evalLightsProgram->setUniformTexVal("textureB", 1);
		evalLightsProgram->setUniformTexVal("textureC", 2);
		evalLightsProgram->setUniformTexVal("textureD", 3);
		evalLightsProgram->setUniformTexVal("textureE", 4);

		evalLightsProgram->setUniformMat4("screen2eyeTf", screen2EyeTf);

		glBindVertexArray(fullscreenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion

#pragma region Update
		glm::mat3 cameraRotation = glm::mat3_cast(cameraController.getCurrentRotation());
		glm::vec3 cameraViewDirection = settings->mCamera.mPosition - settings->mCamera.mCenterOfInterest;
		glm::vec3 cameraPosition = settings->mCamera.mCenterOfInterest + cameraRotation * cameraViewDirection;
		glm::vec3 cameraUpDirection = cameraRotation * settings->mCamera.mUpDirection;
		viewTf = glm::lookAt(cameraPosition, settings->mCamera.mCenterOfInterest, cameraUpDirection);
#pragma endregion

		window.display();
	}

	return 0;
}