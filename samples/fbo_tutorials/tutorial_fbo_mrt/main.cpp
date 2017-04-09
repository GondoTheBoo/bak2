#include "../../../common/opengl.h"
#include "../../../common/sfml.h"
#include "../../../common/glm.h"

#include "../../../common/glsl.h"
#include "../../../common/arcball.h"
#include "../../../common/triasset.h"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
#pragma region InitWindowAndOpenGLContext
	// 800x800 window, 16 bits depth buffer, 0 bits stencil buffer, no multisampling, OpenGL version 3.3, core profile
	const unsigned windowWidth = 768;
	const unsigned windowHeight = 768;
	sf::Window window(sf::VideoMode(windowWidth, windowHeight, 32), "gaudeamus igitur", sf::Style::Close, sf::ContextSettings( 16, 0, 0, 3, 3, sf::ContextSettings::Attribute::Core ));
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: could not initialize opengl extensions" << std::endl;
		return -1;
	}
#pragma endregion

#pragma region InitCameraMatrices
	glm::vec3 initialCameraPosition(0.f, 0.f, 3.5f);
	glm::vec3 cameraCenterOfInterest(0.f, 0.f, 0.f);
	glm::vec3 initialCameraUpwardsDirection(0.f, 1.f, 0.f);
	glm::mat4 viewTf = glm::lookAt(initialCameraPosition, cameraCenterOfInterest, initialCameraUpwardsDirection);
	float cameraFieldOfView = 30.f;
	float cameraNearPlane = 0.1f;
	float cameraFarPlane = 5.f;
	float cameraAspectRatio = windowWidth / static_cast<float>(windowHeight);	// camera aspect = window aspect
	glm::mat4 projectionTf = glm::perspective(glm::radians(cameraFieldOfView), cameraAspectRatio, cameraNearPlane, cameraFarPlane);	// --> perspective expects field of view param in radians
#pragma endregion

#pragma region ImportModel
	char * toDataEnv = std::getenv("VCG_2016_DATA_DIR");
	fs::path dataPath = (toDataEnv != nullptr ? toDataEnv : "../data");
	fs::path modelFile = dataPath; modelFile.append("/models/ogre/bs_angry.obj");
	std::shared_ptr< const cg2::TriAsset > model = cg2::TriAsset::import(modelFile);
#pragma endregion

#pragma region DefineFullscreenQuad
	// as per the OpenGL spec, a vertex array object is necessary for rendering at all times
	// this w create an empty VAO for the fullscreen quad geometry --> attributeless rendering
	// please check the vertex shader 'tut_fbo_renderpass2.vert' to see how the quad vertices are generated
	GLuint fullscreenQuad = 0;
	glGenVertexArrays(1, &fullscreenQuad);
#pragma endregion

#pragma region LoadGlslShaders
	fs::path fragmentShaderFile1 = "shaders/tutorials/tut_fbo_mrt_renderpass1.frag";
	fs::path vertexShaderFile1 = "shaders/tutorials/tut_fbo_mrt_renderpass1.vert";
	fs::path fragmentShaderFile2 = "shaders/tutorials/tut_fbo_mrt_renderpass2.frag";
	fs::path vertexShaderFile2 = "shaders/tutorials/tut_fbo_mrt_renderpass2.vert";
	std::shared_ptr< cg2::GlslProgram > glslProgram_Renderpass1 = cg2::GlslProgram::create( { cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource(vertexShaderFile1)), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource(fragmentShaderFile1)) }, true);
	std::shared_ptr< cg2::GlslProgram > glslProgram_Renderpass2 = cg2::GlslProgram::create({ cg2::ShaderInfo(cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource(vertexShaderFile2)), cg2::ShaderInfo(cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource(fragmentShaderFile2)) }, true);
#pragma endregion

#pragma region CreateFBO
	const unsigned int fboWidth = 768;
	const unsigned int fboHeight = 768;
	GLuint handleToFramebufferObject = 0;
	GLuint handleToDepthTexture = 0;
	GLuint handleToGoochTexture = 0;
	GLuint handleToGreyscaleTexture = 0;

	// step1: create & init textures
	glGenTextures(1, std::addressof(handleToDepthTexture));
	glBindTexture(GL_TEXTURE_2D, handleToDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenTextures(1, std::addressof(handleToGoochTexture));
	glBindTexture(GL_TEXTURE_2D, handleToGoochTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenTextures(1, std::addressof(handleToGreyscaleTexture));
	glBindTexture(GL_TEXTURE_2D, handleToGreyscaleTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// step 2: create & bind fbo
	glGenFramebuffers(1, std::addressof(handleToFramebufferObject));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToFramebufferObject);

	// step 3: attach buffers to fbo
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, handleToDepthTexture, 0);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, handleToGoochTexture, 0);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, handleToGreyscaleTexture, 0);

	// step 4: configure draw buffers for this fbo
	// fragment shader output location 0 will be put into GL_COLOR_ATTACHMENT0
	// fragment shader output location 1 will be put into GL_COLOR_ATTACHMENT1
	// everything greater than 1 will be discarded (GL_NONE)
	std::vector< GLenum > fboBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(fboBuffers.size(), fboBuffers.data());

	// step 5: completeness check
	GLenum statusCode = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (statusCode != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer invalid, status code " << statusCode << std::endl;
		return -1;
	}

	// all done, unbind
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#pragma endregion

	cg2::Arcball cameraRotation(glm::uvec2(0, 0), glm::uvec2(windowWidth, windowHeight));
	cameraRotation.setConstraintAxis(glm::vec3(0.f, 1.f, 0.f)); cameraRotation.toggleConstraintUsage();

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
			else if (sf::Event::MouseMoved == ev.type)
			{
				cameraRotation.updateRotation(glm::uvec2(ev.mouseMove.x, ev.mouseMove.y));
			}
		}
		if (shouldExit)
		{
			window.close();
			break;
		}
#pragma endregion

#pragma region Renderpass1
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handleToFramebufferObject);
		glViewport(0, 0, fboWidth, fboHeight);

		const glm::vec4 white(1.f, 1.f, 1.f, 1.f);
		const glm::vec4 black(0.f, 0.f, 0.f, 0.f);
		const float one = 1.f;
		glClearBufferfv(GL_COLOR, 0, &white[0]);
		//glClearBufferfv(GL_COLOR, 1, &black[0]);	// i commented this line because the result is fun, however this is how you clear color attachment 1 -- 2nd arg is attachment index
		glClearBufferfv(GL_DEPTH, 0, &one);

		glEnable(GL_DEPTH_TEST);

		cg2::GlslProgram::setActiveProgram(glslProgram_Renderpass1);
		glm::vec4 lightPosition = viewTf * glm::vec4(2.f, 2.f, 2.f, 1.f);
		glslProgram_Renderpass1->setUniformVec4("lightPosition", lightPosition.x, lightPosition.y, lightPosition.z, 1.f);
		glslProgram_Renderpass1->setUniformVec3("warmColor", 0.99f, 0.5f, 0.33f);
		glslProgram_Renderpass1->setUniformVec3("coolColor", 0.33f, 0.5f, 0.99f);
		glslProgram_Renderpass1->setUniformVal("blendFactor", 0.66f);
		glslProgram_Renderpass1->setUniformVal("shininess", 10.f);

		for (auto node : model->mScenegraph)				// each asset, no matter how simple, contains a full scenegraph
		{
			glm::mat4 modelTf = node->mModelTf;
			glm::mat4 modelviewTf = viewTf * modelTf;
			glm::mat4 modelviewprojectionTf = projectionTf * modelviewTf;

			glm::mat3 normals2EyeTf = glm::mat3(glm::inverse(glm::transpose(modelviewTf)));

			glslProgram_Renderpass1->setUniformMat4("projectionTf", projectionTf);
			glslProgram_Renderpass1->setUniformMat4("modelviewTf", modelviewTf);
			glslProgram_Renderpass1->setUniformMat3("normals2EyeTf", normals2EyeTf);

			for (unsigned id : node->mMeshIds)				// each node within the scengraph may reference 0..N meshes which should be drawn
			{
				auto meshData = model->mMeshDataCPU[id];	// needed b/c it stores the index count
				unsigned vao = model->mVAOs[id];			// vertex array object

				glBindVertexArray(vao);
				glVertexAttrib3f(8, 0.66f, 0.66f, 0.66f);
				glDrawElements(GL_TRIANGLES, meshData.mIndexCount, GL_UNSIGNED_INT, nullptr);
			}
		}

		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#pragma endregion

#pragma region Renderpass2		
		glViewport(0, 0, windowWidth, windowHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, handleToGoochTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, handleToGreyscaleTexture);

		cg2::GlslProgram::setActiveProgram(glslProgram_Renderpass2);
		glslProgram_Renderpass2->setUniformTexVal("colorTex", 0);
		glslProgram_Renderpass2->setUniformTexVal("greyscaleTex", 1);
		glslProgram_Renderpass2->setUniformVec3("edgeColor", 0.f, 0.f, 0.f);
		glslProgram_Renderpass2->setUniformVal("edgeThreshold", 0.5f);

		glBindVertexArray(fullscreenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		cg2::GlslProgram::setActiveProgram(nullptr);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		cg2::GlslProgram::setActiveProgram(glslProgram_Renderpass2);
#pragma endregion

#pragma region Update
		glm::mat3 currentCameraRotation = glm::mat3_cast(cameraRotation.getCurrentRotation());
		glm::vec3 cameraPosition = currentCameraRotation * initialCameraPosition;
		glm::vec3 cameraUpwardsDirection = currentCameraRotation * initialCameraUpwardsDirection;
		viewTf = glm::lookAt(cameraPosition, cameraCenterOfInterest, cameraUpwardsDirection);
#pragma endregion

#pragma region Display
		window.display();
#pragma endregion
	}

	return 0;
}