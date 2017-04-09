#include "../../../common/opengl.h"	// -> all relevant opengl headers + glew
#include "../../../common/sfml.h"		// -> all relevant sfml headers
#include "../../../common/glm.h"		// -> all relevant glm ( maths ) headers

#include "../../../common/glsl.h"

#include <iostream>
#include <string>

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

// load and create tutorial shader using the cg2::GlslProgram class

	std::vector< cg2::ShaderInfo > tutorialShaders = { { cg2::ShaderType::VERTEX_SHADER, cg2::loadShaderSource("shaders/tutorials/tutorial1.vert") },{ cg2::ShaderType::FRAGMENT_SHADER, cg2::loadShaderSource("shaders/tutorials/tutorial1.frag") } };
	std::shared_ptr< cg2::GlslProgram > tutorialProg = cg2::GlslProgram::create(tutorialShaders, true);

// define & upload geometry to GPU

	// general note:
	// a std::vector is an array that can be resized dynamically, e.g. you can easily append values
	// we'll be using std::vector A LOT. make sure you know what the std::vector methods do
	// and by this i mean: LOOK THEM UP! at http://www.cplusplus.com/reference/vector/vector/
	std::vector< unsigned > indexData = { 0, 6, 1, 1, 6, 2, 2, 6, 3, 3, 6, 4, 4, 6, 5, 5, 6, 0 };

	// this may be of interest: http://stackoverflow.com/questions/879664/is-there-a-difference-between-a-struct-in-c-and-a-struct-in-c
	// anyway, i like using structs to represent 'data', as opposed to classes, which have non-trivial logic. but this is just MY personal convention.
	// also, this construct qualifies as 'PLAIN OLD DATA', which means that having an array of VertexData
	// will be contiguous in memory, and the layout will be exactly as we expect it:
	// 3 floats ( aka 12 bytes ) to represent the position, followed by 4 floats ( aka 16 bytes ) to represent the color.
	// THIS FACT IS SUPER IMPORNTANT; OTHERWISE WE ABSOLUTELY CANNOT PERFORM AN UPLOAD @GPU
	struct VertexData
	{
		glm::vec3 position;
		glm::vec4 color;
		// potential attribs: glm::vec3 normal, glm::vec2 uv, glm::ivec4 boneIndices, etc...
	};

	std::vector< VertexData > attribData =	{
												{ glm::vec3(  4.f,  0.f, 0.f ), glm::vec4( 1.f, 0.f, 0.f, 1.f ) },	// A
												{ glm::vec3(  2.f, -3.f, 0.f ), glm::vec4( 1.f, 1.f, 0.f, 1.f ) },	// B
												{ glm::vec3( -2.f, -3.f, 0.f ), glm::vec4( 0.f, 1.f, 0.f, 1.f ) },	// C
												{ glm::vec3( -4.f,  0.f, 0.f ), glm::vec4( 0.f, 1.f, 1.f, 1.f ) },	// D
												{ glm::vec3( -2.f,  3.f, 0.f ), glm::vec4( 0.f, 0.f, 1.f, 1.f ) },	// E
												{ glm::vec3(  2.f,  3.f, 0.f ), glm::vec4( 1.f, 0.f, 0.f, 1.f ) },	// F
												{ glm::vec3(  0.f,  0.f, 0.f ), glm::vec4( 1.f, 1.f, 1.f, 1.f ) }	// G
											};

	GLuint handleToIndexBuffer = 0;
	glGenBuffers( 1, std::addressof( handleToIndexBuffer ) );	// or: glCreateBuffer( 1, &handleToIndexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, handleToIndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned ) * indexData.size(), reinterpret_cast< GLvoid const* >( indexData.data() ), GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	GLuint handleToAttribBuffer = 0;
	glGenBuffers( 1, std::addressof( handleToAttribBuffer ) );	// or: glCreateBuffer( 1, &handleToAttribBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, handleToAttribBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof( VertexData ) * attribData.size(), reinterpret_cast< GLvoid const* >( attribData.data() ), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// soooo... facts:
	//
	// sizeof gives you the size in bytes of a data type
	// -> sizeof( T ) multiplied by std::vector< T >::size() = size in bytes of the whole std::vector< T >
	//
	// std::adressof( X ) gives you a pointer to the variable X, and is the same as writing &X
	// OpenGL wants complex objects passed via a pointer to their location in memory ALL THE TIME
	// we do it again in glBufferData: std::vector< T >::data() -> gives you a pointer to the first T in the array
	//
	// what the reinterpret_cast to GLvoid const* should tell you, is that OpenGL has absolutely no clue
	// what kind of data we just uploaded into a buffer; on the GPU it's just a bunch of bytes without any meaning.

// vertex attrib setup

	GLuint handleToVertexArray = 0;
	glGenVertexArrays( 1, std::addressof( handleToVertexArray ) );
	glBindVertexArray( handleToVertexArray );

	// ELEMENT_ARRAY_BUFFER state is stored in vertex array object
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, handleToIndexBuffer );

	// ARRAY_BUFFER state is stored in vertex array object
	glBindBuffer( GL_ARRAY_BUFFER, handleToAttribBuffer );

	// vertex attrib pointer state is stored in vertex array object
	// attrib pointer always refers to the current array buffer target.
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, sizeof( VertexData ), reinterpret_cast< GLvoid const * >( offsetof( VertexData, position ) ) );
	glVertexAttribPointer( 8, 4, GL_FLOAT, false, sizeof( VertexData ), reinterpret_cast< GLvoid const * >( offsetof( VertexData, color ) ) );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 8 );

	glBindVertexArray( 0 );

// matrices & associated variables
	const float distToOrigin = 15.f;	// see pdf for an explanation on how to get this value
	glm::mat4 viewTf = glm::lookAt( glm::vec3( 0.f, 0.f, distToOrigin ), glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ) );
	const glm::mat4 projectionTf = glm::perspective( glm::radians( 30.f ), 1.f, 0.1f, 100.f );

// timer for rotation
	sf::Clock timer;

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

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_MULTISAMPLE );

		glViewport( 0, 0, windowWidth, windowHeight );

		glClearColor( 1.f, 1.f, 1.f, 1.f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		cg2::GlslProgram::setActiveProgram( tutorialProg );
		tutorialProg->setUniformMat4( "modelTf", glm::mat4( 1.f ) );
		tutorialProg->setUniformMat4( "viewTf", viewTf );
		tutorialProg->setUniformMat4( "projectionTf", projectionTf );

		glBindVertexArray( handleToVertexArray );
		glDrawElements( GL_TRIANGLES, indexData.size(), GL_UNSIGNED_INT, nullptr );
		glBindVertexArray( 0 );

		cg2::GlslProgram::setActiveProgram( nullptr );

		glDisable( GL_DEPTH_TEST );
		glDisable( GL_MULTISAMPLE );

		////////////////////////////////////////////////////////////////////////////////////////
		// UPDATE: PERFORM CALCULATIONS @CPU FOR NEXT FRAME
		////////////////////////////////////////////////////////////////////////////////////////

		glm::mat3 rotation = glm::mat3( glm::rotate( glm::mat4( 1.f ), glm::radians( 5.f * timer.getElapsedTime().asSeconds() ), glm::vec3( 0.f, 1.f, 0.f ) ) );
		glm::vec3 cameraPosition = rotation * glm::vec3( 0.f, 0.f, distToOrigin );
		viewTf = glm::lookAt( cameraPosition, glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ) );

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
	glDeleteBuffers( 1, std::addressof( handleToIndexBuffer ) );
	glDeleteBuffers( 1, std::addressof( handleToAttribBuffer ) );

	glDeleteVertexArrays( 1, std::addressof( handleToVertexArray ) );

	// GlslPorgram 'tutorialProgram' is destroied when the shared_ptr falls out of scope
	// which will happen on return.

	return 0;
}