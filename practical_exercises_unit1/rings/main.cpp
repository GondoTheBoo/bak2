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
	// 800x800 window, 16 bits depth buffer, 0 bits stencil buffer, 4 x multisampling, OpenGL version 3.3, core profile
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
	glm::vec3 cameraPosition(0.f, 0.f, 3.f);
	glm::vec3 cameraCenterOfInterest(0.f, 0.f, 0.f);
	glm::vec3 cameraUpwardsDirection(0.f, 1.f, 0.f);
	glm::mat4 viewTf = glm::lookAt(cameraPosition, cameraCenterOfInterest, cameraUpwardsDirection);
	float cameraFieldOfView = 30.f;
	float cameraNearPlane = 0.1f;
	float cameraFarPlane = 10.f;
	float cameraAspectRatio = windowWidth / static_cast<float>(windowHeight);	// camera aspect = window aspect
	glm::mat4 projectionTf = glm::perspective(glm::radians(cameraFieldOfView), cameraAspectRatio, cameraNearPlane, cameraFarPlane);	// --> perspective expects field of view param in radians
#pragma endregion

#pragma region ImportModel
	char *toDataEnv = std::getenv("VCG_2016_DATA_DIR");
	fs::path dataPath = (toDataEnv != nullptr ? toDataEnv : "../data");
	fs::path modelFile = dataPath; modelFile.append("/models/rings/rings.obj");
	std::shared_ptr< const cg2::TriAsset > model = cg2::TriAsset::import(modelFile);
#pragma endregion

#pragma region ImportTextures
	fs::path texAFile = dataPath; texAFile.append("/textures/iris.png");
	fs::path texBFile = dataPath; texBFile.append("/textures/pattern.png");
	auto texA = cg2::Texture2D::import(texAFile, cg2::TextureFilter::Trilinear);
	auto texB = cg2::Texture2D::import(texBFile, cg2::TextureFilter::Trilinear);
#pragma endregion

#pragma region LoadGlslShaders
	fs::path fragmentShaderFile = "shaders/practical_exercises_unit1/ex2_rings.frag";
	fs::path vertexShaderFile = "shaders/practical_exercises_unit1/ex2_rings.vert";
	std::shared_ptr< cg2::GlslProgram > glslProgram = cg2::GlslProgram::create( { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource(vertexShaderFile)), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource(fragmentShaderFile)) }, true);
#pragma endregion

	cg2::Arcball modelRotation(glm::uvec2(0, 0), glm::uvec2(windowWidth, windowHeight));
	sf::Clock timer;

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
				modelRotation.startRotation(glm::uvec2(ev.mouseButton.x, ev.mouseButton.y));
			}
			else if (sf::Event::MouseButtonReleased == ev.type && ev.mouseButton.button == sf::Mouse::Left)
			{
				modelRotation.endRotation();
			}
			else if (sf::Event::MouseMoved == ev.type)
			{
				modelRotation.updateRotation(glm::uvec2(ev.mouseMove.x, ev.mouseMove.y));
			}
			else if (sf::Event::KeyPressed == ev.type)
			{
				switch (ev.key.code)
				{
				case sf::Keyboard::Space:
					glslProgram = cg2::GlslProgram::create({ cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource(vertexShaderFile)), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource(fragmentShaderFile)) }, true);
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

		glEnable(GL_MULTISAMPLE);

		float clearDepth = 1.f;
		glm::vec4 clearColor(0.f, 0.f, 0.f, 1.f);
		glClearBufferfv(GL_DEPTH, 0, &clearDepth);
		glClearBufferfv(GL_COLOR, 0, &clearColor[0]);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);

		cg2::GlslProgram::setActiveProgram(glslProgram);
		glslProgram->setUniformVal("time", timer.getElapsedTime().asSeconds());

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		for (auto node : model->mScenegraph)				// each asset, no matter how simple, contains a full scenegraph
		{
			glm::mat4 modelTf = glm::mat4_cast(modelRotation.getCurrentRotation()) * node->mModelTf;
			glm::mat4 modelviewTf = viewTf * modelTf;
			glm::mat4 modelviewprojectionTf = projectionTf * modelviewTf;

			for (unsigned id : node->mMeshIds)				// each node within the scengraph may reference 0..N meshes which should be drawn
			{
				auto meshData = model->mMeshDataCPU[id];	// needed b/c it stores the index count
				unsigned vao = model->mVAOs[id];			// vertex array object
				
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
			}
		}
#pragma endregion

#pragma region Update
		// nothing to do
#pragma endregion

#pragma region Display
		window.display();	// -> calls glFinish
#pragma endregion
	}

	return 0;
}