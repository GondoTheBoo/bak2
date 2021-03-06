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
	std::string settingsFile = ( ( argc < 2 ) ? "settings\\ogre_pointlights.txt" : argv[1] );
	std::shared_ptr< const cg2::Settings > settings = cg2::Settings::load( settingsFile );
	if (settings == nullptr)
	{
		std::cerr << "settings file not found" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitWindowAndOpenGL
	sf::Window window( sf::VideoMode( settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight, 32 ), "deferred w proxies", sf::Style::Close, sf::ContextSettings( 16, 0, 0, 3, 3 ) );
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "glew init failed" << std::endl;
		return -1;
	}
#pragma endregion


#pragma region InitCameraMatrices
	const float aspectRatio = settings->mGeneral.mWindowWidth / static_cast<float>(settings->mGeneral.mWindowHeight);
	glm::mat4 viewTf = glm::lookAt(settings->mCamera.mPosition, settings->mCamera.mCenterOfInterest, settings->mCamera.mUpDirection);
	const glm::mat4 projectionTf = glm::perspective(glm::radians(settings->mCamera.mFoV), aspectRatio, settings->mCamera.mZNear, settings->mCamera.mZFar);
	const glm::mat4 viewportTf = glm::mat4(glm::vec4(settings->mGeneral.mWindowWidth / 2, 0.f, 0.f, 0.f), glm::vec4(0.f, settings->mGeneral.mWindowHeight / 2, 0.f, 0.f), glm::vec4(0.f, 0.f, 0.5f, 0.f), glm::vec4(settings->mGeneral.mWindowWidth / 2, settings->mGeneral.mWindowHeight / 2, 0.5f, 1.f));
	const glm::mat4 screen2EyeTf = glm::inverse(viewportTf * projectionTf);	// needed to reconstruct position from depth
#pragma endregion

#pragma region InitRsmMatrices
	const glm::mat4 lightViewTf = glm::lookAt(settings->mLights[0].mPosition, settings->mCamera.mCenterOfInterest, settings->mCamera.mUpDirection);
	const glm::mat4 lightProjectionTf = glm::perspective(glm::radians(settings->mCamera.mFoV), aspectRatio, settings->mCamera.mZNear, settings->mCamera.mZFar);
	const glm::mat4 lightWorld2Screen = viewportTf * lightProjectionTf * lightViewTf;
	const glm::mat4 lightScreen2WorldTf = glm::inverse(lightWorld2Screen);	// needed to reconstruct position from depth
#pragma endregion


#pragma region LoadGlslShaders
	std::vector< cg2::ShaderInfo > buildGBufferShaders = { cg2::ShaderInfo( cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource( "shaders\\solution_deferred\\buildGBuffer.vert" ) ), cg2::ShaderInfo( cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource( "shaders\\solution_deferred\\buildGBuffer.frag" ) ) };
	std::shared_ptr< cg2::GlslProgram > buildGBufferProgram = cg2::GlslProgram::create(buildGBufferShaders, true );
	std::vector< cg2::ShaderInfo > evalAmbientAndDirectionalLightsShaders = { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalAmbientAndDirectionalLight.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalAmbientAndDirectionalLight.frag")) };
	std::shared_ptr< cg2::GlslProgram > evalAmbientAndDirectionalLightsProgram = cg2::GlslProgram::create(evalAmbientAndDirectionalLightsShaders, true);
	std::vector< cg2::ShaderInfo > evalPointLightShaders = { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalPointLight.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalPointLight.frag")) };
	std::shared_ptr< cg2::GlslProgram > evalPointLightProgram = cg2::GlslProgram::create(evalPointLightShaders, true);
	std::vector< cg2::ShaderInfo > evalSpotLightShaders = { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalSpotLight.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\evalSpotLight.frag")) };
	std::shared_ptr< cg2::GlslProgram > evalSpotLightProgram = cg2::GlslProgram::create(evalSpotLightShaders, true);
	std::vector< cg2::ShaderInfo > buildRsmShaders = { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\buildRsm.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\buildRsm.frag")) };
	std::shared_ptr< cg2::GlslProgram > buildRsmProgram = cg2::GlslProgram::create(buildRsmShaders, true);
	std::vector< cg2::ShaderInfo > showRsmShaders = { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\showRsm.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders\\solution_deferred\\showRsm.frag")) };
	std::shared_ptr< cg2::GlslProgram > showRsmProgram = cg2::GlslProgram::create(showRsmShaders, true);
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
		asset.sceneTf = glm::translate(glm::mat4(1.f), a.mTranslation) * glm::mat4_cast(a.mOrientation) * glm::scale(glm::mat4(1.f), a.mScaling);
		assets.push_back(asset);
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
		if (i == 0)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	glGenFramebuffers(1, std::addressof(handleToGBuffer));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToGBuffer);

	for (unsigned i = 0; i < handleToGBufferTextures.size(); ++i) {
		if (i == 0)
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, handleToGBufferTextures[i], 0);
		else
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (i - 1), handleToGBufferTextures[i], 0);
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

	//lucas

	GLuint handleToRsmGBuffer = 0;
	std::vector< GLuint > handleToRsmGBufferTextures(4, 0);
	const glm::vec4 texDefaultRsm(0.f, 0.f, 0.f, 0.f);

	//generate texture handle and bind texture
	for (unsigned i = 0; i < handleToRsmGBufferTextures.size(); ++i) {
		glGenTextures(1, std::addressof(handleToRsmGBufferTextures[i]));
		glBindTexture(GL_TEXTURE_2D, handleToRsmGBufferTextures[i]);
		if (i == 0)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	//generate and bind handle of Rsm buffer
	glGenFramebuffers(1, std::addressof(handleToRsmGBuffer));

	//bind drawbuffer and textures
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToRsmGBuffer);

	//assign the depth values, and colors to the handles from before
	for (unsigned i = 0; i < handleToRsmGBufferTextures.size(); ++i) {
		if (i == 0)
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, handleToRsmGBufferTextures[i], 0);
		else
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (i - 1), handleToRsmGBufferTextures[i], 0);
	}


	//add this line and replace, if i want to render to other color attachments etc... differnet than previous setup
	std::vector< GLenum > rsmFboBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(rsmFboBuffers.size(), rsmFboBuffers.data());

	GLenum statusCodeRsm = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (statusCodeRsm != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer invalid, status code " << statusCodeRsm << std::endl;
		return -1;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// end lucas


#pragma endregion

	cg2::Arcball cameraController(glm::uvec2(0, 0), glm::uvec2(settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowWidth));
	cameraController.setConstraintAxis(glm::vec3(0.f, 1.f, 0.f)); cameraController.toggleConstraintUsage();

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
		
#pragma region RSMPass
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToRsmGBuffer);
		const float one = 1.f;
		glClearBufferfv(GL_DEPTH, 0, &one);
		glClearBufferfv(GL_COLOR, 0, &texDefaultRsm[0]);
		glClearBufferfv(GL_COLOR, 1, &texDefaultRsm[0]);
		glClearBufferfv(GL_COLOR, 2, &texDefaultRsm[0]);

		glViewport(0, 0, fboWidth, fboHeight);

		cg2::GlslProgram::setActiveProgram(buildRsmProgram);
		for (auto asset : assets)
		{
			for (auto node : asset.model->mScenegraph)
			{
				glm::mat4 modelTf = asset.sceneTf * node->mModelTf;
				glm::mat4 modelviewTf = lightViewTf * modelTf;
				glm::mat4 mvpTf = lightProjectionTf * lightViewTf * modelTf;
				float lightIntensity = settings->mLights[0].mIntensity;
				glm::vec3 lightPos = settings->mLights[0].mPosition;
				
				buildRsmProgram->setUniformMat4("mTf", modelTf);
				buildRsmProgram->setUniformMat4("mvpTf", mvpTf);
				buildRsmProgram->setUniformVal("lightIntensity", lightIntensity);
				buildRsmProgram->setUniformVec3("lightPos", lightPos);
				glm::mat3 normals2WorldTf = glm::mat3(glm::inverse(glm::transpose(modelTf)));
				buildRsmProgram->setUniformMat3("normals2WorldTf", normals2WorldTf);

				for (auto id : node->mMeshIds)
				{
					auto meshData = asset.model->mMeshDataCPU[id];
					auto vao = asset.model->mVAOs[id];

					if (!meshData.mMaterial.mDiffuseTexture.empty())
					{
						auto texture = asset.model->mTextures.at(meshData.mMaterial.mDiffuseTexture);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, texture->mHandle);
						buildRsmProgram->setUniformBVal("hasDiffuseTexture", true);
					}
					else
					{
						buildRsmProgram->setUniformBVal("hasDiffuseTexture", false);
					}

					buildRsmProgram->setUniformTexVal("diffuseTexture", 0);
					buildRsmProgram->setUniformVec3("kDiffuse", meshData.mMaterial.mKDiffuse);

					glBindVertexArray(vao);
					glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
				}
			}
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight);
#pragma endregion


#pragma region RenderPass1
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToGBuffer);
		//const float one = 1.f;
		glClearBufferfv(GL_DEPTH, 0, &one);
		glClearBufferfv(GL_COLOR, 0, &normalsDefault[0]);
		glClearBufferfv(GL_COLOR, 1, &diffuseDefault[0]);
		glClearBufferfv(GL_COLOR, 2, &specularDefault[0]);
		glClearBufferfv(GL_COLOR, 3, &ambientDefault[0]);

		glViewport(0, 0, fboWidth, fboHeight);

		cg2::GlslProgram::setActiveProgram(buildGBufferProgram);
		buildGBufferProgram->setUniformIVal("solidFlag", 0);
		for (auto asset : assets)
		{
			for (auto node : asset.model->mScenegraph)
			{
				glm::mat4 modelTf = asset.sceneTf * node->mModelTf;
				glm::mat4 modelviewTf = viewTf * modelTf;
				glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
				glm::mat3 normals2eyeTf = glm::mat3(glm::inverse(glm::transpose(modelviewTf)));

				buildGBufferProgram->setUniformMat4("mvpTf", mvpTf);
				buildGBufferProgram->setUniformMat3("normals2eyeTf", normals2eyeTf);

				for (auto id : node->mMeshIds)
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

					glBindVertexArray(vao);
					glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
				}
			}
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);


		/*
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		buildGBufferProgram->setUniformIVal("solidFlag", 1);

		for (auto light : settings->mLights)
		{
			if (light.mType == LightType::Point)
			{
				float scale = 0.2f*light.mIntensity;
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
				float scale = 0.2f*light.mIntensity;
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
		*/
		glDisable(GL_DEPTH_TEST);
#pragma endregion

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, settings->mGeneral.mWindowWidth, settings->mGeneral.mWindowHeight);
		
		//bind G-buffer
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
		
		//bind Rsm textures
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, handleToRsmGBufferTextures[0]);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, handleToRsmGBufferTextures[1]);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, handleToRsmGBufferTextures[2]);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, handleToRsmGBufferTextures[3]);

		/*
#pragma region RenderRsmData
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_TRUE);

		cg2::GlslProgram::setActiveProgram(showRsmProgram);
		showRsmProgram->setUniformTexVal("tex_rsm_depth", 5);
		showRsmProgram->setUniformTexVal("tex_rsm_normal", 6);
		showRsmProgram->setUniformTexVal("tex_rsm_worldPos", 7);
		showRsmProgram->setUniformTexVal("tex_rsm_intensity", 8);

		glBindVertexArray(fullscreenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glDisable(GL_DEPTH_TEST);
#pragma endregion
		*/
		
#pragma region AmbientAndDirectionalPass
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glDepthMask(GL_TRUE);

		cg2::GlslProgram::setActiveProgram(evalAmbientAndDirectionalLightsProgram);
		evalAmbientAndDirectionalLightsProgram->setUniformVal("ambientIntensity", settings->mEnvironment.mAmbientIntensity);
		evalAmbientAndDirectionalLightsProgram->setUniformVec3("ambientColor", settings->mEnvironment.mAmbientColor);
		evalAmbientAndDirectionalLightsProgram->setUniformTexVal("texDepth", 0);
		evalAmbientAndDirectionalLightsProgram->setUniformTexVal("texDiff", 2);
		evalAmbientAndDirectionalLightsProgram->setUniformTexVal("texAmb", 4);
		evalAmbientAndDirectionalLightsProgram->setUniformMat4("screen2eyeTf", screen2EyeTf);

		glBindVertexArray(fullscreenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glDisable(GL_DEPTH_TEST);
#pragma endregion
		
#pragma region PointLightPass
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
		glDepthMask(GL_FALSE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		

		cg2::GlslProgram::setActiveProgram(evalPointLightProgram);
		evalPointLightProgram->setUniformTexVal("textureA", 0);
		evalPointLightProgram->setUniformTexVal("textureB", 1);
		evalPointLightProgram->setUniformTexVal("textureC", 2);
		evalPointLightProgram->setUniformMat4("screen2eyeTf", screen2EyeTf);
		evalPointLightProgram->setUniformTexVal("tex_rsm_depth", 5);
		evalPointLightProgram->setUniformTexVal("tex_rsm_normal", 6);
		//evalPointLightProgram->setUniformTexVal("tex_rsm_worldPos", 7);
		evalPointLightProgram->setUniformTexVal("tex_rsm_intensity", 8);
		evalPointLightProgram->setUniformMat4("lightScreen2WorldTf", lightScreen2WorldTf);
		
		glm::mat3 normals2EyeTf = glm::mat3(glm::inverse(glm::transpose(viewTf)));
		evalPointLightProgram->setUniformMat3("normals2EyeTf", normals2EyeTf);

		for (auto node : pointlightModel->mScenegraph)
		{
			glm::mat4 vpTf = projectionTf * viewTf;
			evalPointLightProgram->setUniformMat4("vTf", viewTf);
			evalPointLightProgram->setUniformMat4("vpTf", vpTf);
			for (auto id : node->mMeshIds)
			{
				auto meshData = pointlightModel->mMeshDataCPU[id];
				auto vao = pointlightModel->mVAOs[id];
				glBindVertexArray(vao);
				// every 64 line and row = 16 lines and 12 rows --> 16*12 = 192
				//glDrawElementsInstanced(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr, 192);

				// every 32 line and row = 32 lines and 24 rows --> 768
				glDrawElementsInstanced(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr, 768);

				// every 16 line and row = 64 lines and 48 rows --> 3072
				//glDrawElementsInstanced(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr, 3072);

				// every 8 line and row = 128 lines and 96 rows --> 12288
				//glDrawElementsInstanced(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr, 12288);
			}
		}


		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

#pragma endregion
		
		
#pragma region SpotLightPass
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
		glDepthMask(GL_FALSE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		cg2::GlslProgram::setActiveProgram(evalSpotLightProgram);

		const glm::mat4 eye2worldTf = glm::inverse(viewTf);

		evalSpotLightProgram->setUniformTexVal("texDepth", 0);
		evalSpotLightProgram->setUniformTexVal("texNormal", 1);
		evalSpotLightProgram->setUniformTexVal("texDiff", 2);
		evalSpotLightProgram->setUniformTexVal("texSpec", 3);
		evalSpotLightProgram->setUniformTexVal("tex_rsm_depth", 5);
		evalSpotLightProgram->setUniformTexVal("tex_rsm_normal", 6);
		evalSpotLightProgram->setUniformTexVal("tex_rsm_diff", 7);
		evalSpotLightProgram->setUniformTexVal("tex_rsm_worldCoord", 8);
		evalSpotLightProgram->setUniformMat4("screen2eyeTf", screen2EyeTf);
		evalSpotLightProgram->setUniformMat4("eye2worldTf", eye2worldTf);
		evalSpotLightProgram->setUniformMat4("lightViewProjectionTf", lightWorld2Screen);			
		
		for (auto light : settings->mLights)
		{
			if (light.mType == LightType::Spot)
			{
				glm::vec4 lightPosition = viewTf * glm::vec4(light.mPosition, 1.f);
				glm::vec4 lightDirection = viewTf * glm::vec4(light.mDirection, 0.f);
				evalSpotLightProgram->setUniformVec4("lightSource.position", lightPosition);
				evalSpotLightProgram->setUniformVec3("lightSource.direction", glm::vec3(lightDirection));
				evalSpotLightProgram->setUniformVal("lightSource.intensity", light.mIntensity);
				evalSpotLightProgram->setUniformVec3("lightSource.color", light.mColor);
				evalSpotLightProgram->setUniformVal("lightSource.cosangle", glm::cos(glm::radians(light.mAngle)));

				const float epsilon = 0.01f; // matches shader epsilon
				float scale = glm::sqrt(light.mIntensity / epsilon);
				glm::mat4 lightTf = glm::translate(glm::mat4(1.f), light.mPosition) * glm::mat4_cast(glm::fquat(glm::vec3(0.f, 1.f, 0.f), glm::normalize(light.mDirection))) * glm::scale(glm::mat4(1.f), glm::vec3(scale*(1 / glm::tan(glm::radians(90 - light.mAngle))), scale, scale*(1 / glm::tan(glm::radians(90 - light.mAngle)))));

				for (auto node : spotlightModel->mScenegraph)
				{
					glm::mat4 modelTf = lightTf * node->mModelTf;
					glm::mat4 modelviewTf = viewTf * modelTf;
					glm::mat4 mvpTf = projectionTf * viewTf * modelTf;
					glm::mat4 local2lightScreenTf = viewportTf * lightProjectionTf * lightViewTf * modelTf;

					evalSpotLightProgram->setUniformMat4("mvpTf", mvpTf);
					evalSpotLightProgram->setUniformMat4("local2lightScreenTf", local2lightScreenTf);

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

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		
		
#pragma endregion

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

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, 0);

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