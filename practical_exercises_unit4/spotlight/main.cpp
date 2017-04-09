#include "../../common/opengl.h"
#include "../../common/sfml.h"
#include "../../common/glm.h"

#include "../../common/glsl.h"
#include "../../common/arcball.h"
#include "../../common/triasset.h"
#include "../../common/texture2D.h"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
#pragma region InitWindowAndOpenGLContext
	const unsigned windowWidth = 800;
	const unsigned windowHeight = 800;
	sf::Window window(sf::VideoMode(windowWidth, windowHeight, 32), "gaudeamus igitur", sf::Style::Close, sf::ContextSettings( 16, 0, 4, 3, 3, sf::ContextSettings::Attribute::Core ));
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: could not initialize opengl extensions" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitCameraMatrices
	const glm::vec3 initialCameraPosition(0.f, 0.f, 5.f);
	glm::vec3 cameraPosition = initialCameraPosition;
	const glm::vec3 initialCameraUpwardsDirection(0.f, 1.f, 0.f);
	glm::vec3 cameraUpwardsDirection = initialCameraUpwardsDirection;
	glm::mat4 viewTf = glm::lookAt(cameraPosition, glm::vec3(0.f), cameraUpwardsDirection);
	const float cameraAspectRatio = windowWidth / static_cast<float>(windowHeight);
	glm::mat4 projectionTf = glm::perspective(glm::radians(30.f), cameraAspectRatio, 0.5f, 30.f);
#pragma endregion

	char * toDataEnv = std::getenv("VCG_2016_DATA_DIR");
	fs::path dataPath = (toDataEnv != nullptr ? toDataEnv : "../data");

#pragma region ImportModel
	struct Asset
	{
		std::shared_ptr< const cg2::TriAsset > asset;
		glm::mat4 modelTf;
	};
	std::vector< Asset > assets;
	assets.push_back({ cg2::TriAsset::import(dataPath / "/models/ogre/angry_ogre_untextured.obj"), glm::mat4(1.f) });
	assets.push_back({ cg2::TriAsset::import(dataPath / "/models/cubes/dark_cube.obj"), glm::translate(glm::mat4(1.f), glm::vec3(0.f, 9.f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(10.f)) });
#pragma endregion

#pragma region SpotLight
	glm::vec3 initialLightDirection(-1.f);
	struct SpotLight
	{
		glm::vec3 position;
		glm::vec3 direction;
		float angle;			// = degrees
		glm::vec3 color;
		float intensity;
	};
	SpotLight light = { { 0.95f, 0.95f, 0.95f }, initialLightDirection, 20.f,{ 1, 1, 1 }, 5.f };
	// asset used for drawing spot light
	std::shared_ptr< const cg2::TriAsset > lightModel = cg2::TriAsset::import(dataPath / "/models/cones/cone_lowres.obj");
	// modelTf used for drawing spot light
	glm::mat4 lightModelTf = glm::translate(glm::mat4(1.f), light.position) * glm::mat4_cast(glm::fquat(glm::vec3(0.f, 1.f, 0.f), glm::normalize(light.direction))) * glm::scale(glm::mat4(1.f), glm::vec3(0.5f*(1 / glm::tan(glm::radians(90 - light.angle))), 0.5f, 0.5f*(1 / glm::tan(glm::radians(90 - light.angle)))));
	// 2d gobo texture w. bilinear filtering & repeat wrap mode
	std::shared_ptr< const cg2::Texture2D > lightGobo = cg2::Texture2D::import(dataPath / "/textures/gobo.jpg", cg2::TextureFilter::Bilinear, cg2::TextureWrap::Repeat);
#pragma endregion

#pragma region LoadGlslShaders
	std::shared_ptr< cg2::GlslProgram > glslProgram = cg2::GlslProgram::create( { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders/practical_exercises_unit4/spotlight_gobo.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders/practical_exercises_unit4/spotlight_gobo.frag")) }, true);
#pragma endregion

	cg2::Arcball cameraRotation(glm::uvec2(0, 0), glm::uvec2(windowWidth, windowHeight));
	cg2::Arcball lightRotation(glm::uvec2(0, 0), glm::uvec2(windowWidth, windowHeight));

	// Main Loop //////////////////
	while (true)
	{
#pragma region HandleUserInput
		bool shouldExit = false;
		sf::Event ev;
		while (window.pollEvent(ev))
		{
			if (sf::Event::Closed == ev.type)
			{
				shouldExit = true;
				break;
			}
			else if (sf::Event::MouseButtonPressed == ev.type && ev.mouseButton.button == sf::Mouse::Left)
			{
				cameraRotation.startRotation(glm::uvec2(ev.mouseButton.x, ev.mouseButton.y));
			}
			else if (sf::Event::MouseButtonReleased == ev.type && ev.mouseButton.button == sf::Mouse::Left)
			{
				cameraRotation.endRotation();
			}
			else if (sf::Event::MouseButtonPressed == ev.type && ev.mouseButton.button == sf::Mouse::Right)
			{
				lightRotation.startRotation(glm::uvec2(ev.mouseButton.x, ev.mouseButton.y));
			}
			else if (sf::Event::MouseButtonReleased == ev.type && ev.mouseButton.button == sf::Mouse::Right)
			{
				lightRotation.endRotation();
			}
			else if (sf::Event::MouseMoved == ev.type)
			{
				cameraRotation.updateRotation(glm::uvec2(ev.mouseMove.x, ev.mouseMove.y));
				lightRotation.updateRotation(glm::uvec2(ev.mouseMove.x, ev.mouseMove.y));
			}
			else if (sf::Event::KeyPressed == ev.type)
			{
				switch (ev.key.code)
				{
				case sf::Keyboard::Space:
					glslProgram = cg2::GlslProgram::create({ cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders/practical_exercises_unit4/spotlight_gobo.vert")), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders/practical_exercises_unit4/spotlight_gobo.frag")) }, true);
					break;
				case sf::Keyboard::Add:
					light.angle = glm::min(80.f, light.angle + 5);
					break;
				case sf::Keyboard::Subtract:
					light.angle = glm::max(10.f, light.angle - 5);
					break;
				default:
					break;
				}
			}
		}
		if (shouldExit)
		{
			window.close();
			break;
		}
#pragma endregion

#pragma region Draw
		glViewport(0, 0, windowWidth, windowHeight);

		glEnable(GL_DEPTH_TEST);

		const float one = 1.f;
		const glm::vec4 backgroundColor(0.1f, 0.1f, 0.1f, 1.f);
		glClearBufferfv(GL_DEPTH, 0, &one);
		glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

		cg2::GlslProgram::setActiveProgram(glslProgram);

		glslProgram->setUniformVec3("environment.ambientColor", backgroundColor);
		glslProgram->setUniformVal("environment.ambientIntensity", 1.f);

		glslProgram->setUniformVec4("light.position", glm::vec4(light.position,1));
		glslProgram->setUniformVec3("light.color", light.color);
		glslProgram->setUniformVal("light.intensity", light.intensity);
		glslProgram->setUniformVec3("light.direction", glm::normalize(light.direction));
		glslProgram->setUniformVal("light.cosangle", glm::cos(glm::radians(light.angle)));

		glslProgram->setUniformVec4("cameraPosition", glm::vec4(cameraPosition, 1));

		glslProgram->setUniformIVal("material.shadingType", 1);		
		for (auto asset : assets)
		{
			for (auto node : asset.asset->mScenegraph)
			{
				glm::mat4 modelTf = asset.modelTf * node->mModelTf;
				glm::mat4 modelviewTf = viewTf * modelTf;
				glm::mat4 modelviewprojectionTf = projectionTf * modelviewTf;
				glm::mat3 normals2worldTf = glm::mat3(glm::inverse(glm::transpose(modelTf)));

				glslProgram->setUniformMat4("mvpTf", modelviewprojectionTf);
				glslProgram->setUniformMat4("modelTf", modelTf);
				glslProgram->setUniformMat3("normals2worldTf", normals2worldTf);
				
				for (unsigned id : node->mMeshIds)
				{
					auto meshData = asset.asset->mMeshDataCPU[id];
					unsigned vao = asset.asset->mVAOs[id];

					glslProgram->setUniformVec3("material.kDiffuse", meshData.mMaterial.mKDiffuse);
					glslProgram->setUniformVec3("material.kAmbient", meshData.mMaterial.mKAmbient);

					glBindVertexArray(vao);
					glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
				}
			}
		}

		glslProgram->setUniformIVal("material.shadingType", 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		
		for (auto node : lightModel->mScenegraph)
		{
			glm::mat4 modelTf = lightModelTf * node->mModelTf;
			glm::mat4 modelviewTf = viewTf * modelTf;
			glm::mat4 modelviewprojectionTf = projectionTf * modelviewTf;
			glslProgram->setUniformMat4("mvpTf", modelviewprojectionTf);

			for (unsigned id : node->mMeshIds)
			{
				glslProgram->setUniformVec3("material.kDiffuse", light.color);

				auto meshData = lightModel->mMeshDataCPU[id];
				unsigned vao = lightModel->mVAOs[id];
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_DEPTH_TEST);
#pragma endregion

#pragma region Update
		glm::mat3 currentCameraRotation = glm::mat3_cast(cameraRotation.getCurrentRotation());
		cameraPosition = currentCameraRotation * initialCameraPosition;
		cameraUpwardsDirection = currentCameraRotation * initialCameraUpwardsDirection;
		viewTf = glm::lookAt(cameraPosition, glm::vec3(0.f), cameraUpwardsDirection);

		glm::mat3 currentLightRotation = glm::mat3_cast(lightRotation.getCurrentRotation());
		light.direction = currentLightRotation * initialLightDirection;
		lightModelTf = glm::translate(glm::mat4(1.f), light.position) * glm::mat4_cast(glm::fquat(glm::vec3(0.f, 1.f, 0.f), glm::normalize(light.direction))) * glm::scale(glm::mat4(1.f), glm::vec3(0.2f*(1/glm::tan(glm::radians(90-light.angle))), 0.2f, 0.2f*(1 / glm::tan(glm::radians(90 - light.angle)))));
#pragma endregion

#pragma region Display
		window.display();
#pragma endregion
	}

	return 0;
}