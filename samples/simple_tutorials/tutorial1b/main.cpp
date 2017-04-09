#include "../../../common/opengl.h"	// -> all relevant opengl headers + glew
#include "../../../common/sfml.h"		// -> all relevant sfml headers
#include "../../../common/glm.h"		// -> all relevant glm ( maths ) headers

#include "../../../common/glsl.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

int main( int argc, char** argv )
{
	unsigned int windowWidth = 800;
	unsigned int windowHeight = 800;

////////////////////////////////////////////////////////////////////////////////////////
// WINDOW CREATION, ALSO CREATES OPENGL CONTEXT & INITIALIZES GLEW
////////////////////////////////////////////////////////////////////////////////////////
	sf::Window window( sf::VideoMode( windowWidth, windowHeight, 32 ), "tutorial", sf::Style::Close, sf::ContextSettings( 16, 0, 4, 3, 3 ) );
	if ( glewInit() != GLEW_OK ) return 0;

////////////////////////////////////////////////////////////////////////////////////////
// INIT: ALLOCATE OPENGL RESOURCES
////////////////////////////////////////////////////////////////////////////////////////

// NEW: load and create tutorial shader using the cg2::GlslProgram class

	std::vector< cg2::ShaderInfo > tutorialShaders = { { cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders/tutorials/tutorial1.vert") },{ cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders/tutorials/tutorial1.frag") } };
	std::shared_ptr< cg2::GlslProgram > tutorialProg = cg2::GlslProgram::create(tutorialShaders, true);

////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP BEGINS
////////////////////////////////////////////////////////////////////////////////////////
	while ( true )
	{
		bool exit = false;
		sf::Event ev;

		////////////////////////////////////////////////////////////////////////////////////////
		// POLL WINDOW EVENTS -> HANDLE USER INPUT
		// check out the sf::Event class
		// at http://www.sfml-dev.org/documentation/2.1/classsf_1_1Event.php
		// for more event types, e.g. keyboard
		////////////////////////////////////////////////////////////////////////////////////////
		while ( window.pollEvent( ev ) )
		{
			if ( sf::Event::Closed == ev.type )
			{
				exit = true;
				break;
			}
			else if ( sf::Event::MouseMoved == ev.type )
			{
				//std::cout << "mouse moved, new position at ( " << ev.mouseMove.x << " | " << ev.mouseMove.y << " ) " << std::endl;
			}
			else if ( sf::Event::MouseButtonPressed == ev.type )
			{
				std::string button = "unknown";

				switch ( ev.mouseButton.button )
				{
					case sf::Mouse::Left:
						button = "left";
						break;
					case sf::Mouse::Right:
						button = "right";
						break;
					case sf::Mouse::Middle:
						button = "middle";
						break;
					default:
						break;
				}

				std::cout << button + " mouse button down at ( " << ev.mouseButton.x << " | " << ev.mouseButton.y << " ) " << std::endl;
			}
		}
		if ( exit ) break;

		////////////////////////////////////////////////////////////////////////////////////////
		// RENDER: MAKE OPENGL CALLS, START WORK @GPU
		////////////////////////////////////////////////////////////////////////////////////////

// NEW: very basic opengl setup
// in theory, some of these calls could be moved to the 'INIT' phase.
// e.g. if we want to keep a constant viewport for all frames.
// we enable depth testing & multisampling: depth testing, because we nearly always want it, and multisampling because it looks nice.
// we define the viewport so that it spans the whole window.
// we clear depth & color ( color -> white, depth -> 1f which is the default )
// we activate our tutorial program
// and set some uniform values
// NOTHING IS RENDERED YET

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_MULTISAMPLE );

		glViewport( 0, 0, windowWidth, windowHeight );

		glClearColor( 1.f, 1.f, 1.f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		cg2::GlslProgram::setActiveProgram( tutorialProg );
		tutorialProg->setUniformMat4( "modelTf", glm::mat4( 1.f ) );
		tutorialProg->setUniformMat4( "viewTf", glm::mat4( 1.f ) );
		tutorialProg->setUniformMat4( "projectionTf", glm::mat4( 1.f ) );

		// here, we will draw... using glDrawElements

		cg2::GlslProgram::setActiveProgram( nullptr );

		glDisable( GL_DEPTH_TEST );
		glDisable( GL_MULTISAMPLE );

		////////////////////////////////////////////////////////////////////////////////////////
		// UPDATE: PERFORM CALCULATIONS @CPU FOR NEXT FRAME
		////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// DISPLAY: SYNC POINT WITH GPU, UPDATE WINDOW CONTENT
		////////////////////////////////////////////////////////////////////////////////////////
		window.display();
	}
////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP ENDS
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// UN-INIT: RELEASE ALL RESOURCES HERE
////////////////////////////////////////////////////////////////////////////////////////

// NEW: nothing, the glsl program object ist destroied once the shared_ptr falls out of scope
// which will happen on return 0

	return 0;
}