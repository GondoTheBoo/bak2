#include "glsl.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include "windows.h"

namespace cg2
{
	std::string toGLSLType(const GLenum val)
	{
		switch (val)
		{
		case GL_FLOAT : return "float";
		case GL_FLOAT_VEC2 : return "vec2";
		case GL_FLOAT_VEC3 : return "vec3";
		case GL_FLOAT_VEC4: return "vec4";
		case GL_INT : return "int";
		case GL_INT_VEC2 : return "ivec2";
		case GL_INT_VEC3 : return "ivec3";
		case GL_INT_VEC4 : return "ivec4";
		case GL_UNSIGNED_INT : return "unsigned int";
		case GL_UNSIGNED_INT_VEC2 : return "uvec2";
		case GL_UNSIGNED_INT_VEC3 : return "uvec3";
		case GL_UNSIGNED_INT_VEC4 : return "uvec4";
		case GL_BOOL : return "bool";
		//case GL_BOOL_VEC2 : return "bvec2";
		//case GL_BOOL_VEC3 : return "bvec3";
		//case GL_BOOL_VEC4 : return "bvec4";
		case GL_FLOAT_MAT2 : return "mat2";
		case GL_FLOAT_MAT3 : return "mat3";
		case GL_FLOAT_MAT4 : return "mat4";
		case GL_SAMPLER_1D : return "sampler1D";
		case GL_SAMPLER_2D : return "sampler2D";
		case GL_SAMPLER_3D : return "sampler3D";
		case GL_SAMPLER_CUBE : return "samplerCube";
		default: return "UNKNOWN / UNSUPPORTED TYPE";
			//GL_FLOAT_MAT2x3	mat2x3
			//GL_FLOAT_MAT2x4	mat2x4
			//GL_FLOAT_MAT3x2	mat3x2
			//GL_FLOAT_MAT3x4	mat3x4
			//GL_FLOAT_MAT4x2	mat4x2
			//GL_FLOAT_MAT4x3	mat4x3
			//GL_DOUBLE_MAT2	dmat2
			//GL_DOUBLE_MAT3	dmat3
			//GL_DOUBLE_MAT4	dmat4
			//GL_DOUBLE_MAT2x3	dmat2x3
			//GL_DOUBLE_MAT2x4	dmat2x4
			//GL_DOUBLE_MAT3x2	dmat3x2
			//GL_DOUBLE_MAT3x4	dmat3x4
			//GL_DOUBLE_MAT4x2	dmat4x2
			//GL_DOUBLE_MAT4x3	dmat4x3
				//GL_SAMPLER_1D_SHADOW	sampler1DShadow
				//GL_SAMPLER_2D_SHADOW	sampler2DShadow
				//GL_SAMPLER_1D_ARRAY	sampler1DArray
				//GL_SAMPLER_2D_ARRAY	sampler2DArray
				//GL_SAMPLER_1D_ARRAY_SHADOW	sampler1DArrayShadow
				//GL_SAMPLER_2D_ARRAY_SHADOW	sampler2DArrayShadow
				//GL_SAMPLER_2D_MULTISAMPLE	sampler2DMS
				//GL_SAMPLER_2D_MULTISAMPLE_ARRAY	sampler2DMSArray
				//GL_SAMPLER_CUBE_SHADOW	samplerCubeShadow
				//GL_SAMPLER_BUFFER	samplerBuffer
				//GL_SAMPLER_2D_RECT	sampler2DRect
				//GL_SAMPLER_2D_RECT_SHADOW	sampler2DRectShadow
				//GL_INT_SAMPLER_1D	isampler1D
				//GL_INT_SAMPLER_2D	isampler2D
				//GL_INT_SAMPLER_3D	isampler3D
				//GL_INT_SAMPLER_CUBE	isamplerCube
				//GL_INT_SAMPLER_1D_ARRAY	isampler1DArray
				//GL_INT_SAMPLER_2D_ARRAY	isampler2DArray
				//GL_INT_SAMPLER_2D_MULTISAMPLE	isampler2DMS
				//GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	isampler2DMSArray
				//GL_INT_SAMPLER_BUFFER	isamplerBuffer
				//GL_INT_SAMPLER_2D_RECT	isampler2DRect
				//GL_UNSIGNED_INT_SAMPLER_1D	usampler1D
				//GL_UNSIGNED_INT_SAMPLER_2D	usampler2D
				//GL_UNSIGNED_INT_SAMPLER_3D	usampler3D
				//GL_UNSIGNED_INT_SAMPLER_CUBE	usamplerCube
				//GL_UNSIGNED_INT_SAMPLER_1D_ARRAY	usampler2DArray
				//GL_UNSIGNED_INT_SAMPLER_2D_ARRAY	usampler2DArray
				//GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE	usampler2DMS
				//GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY	usampler2DMSArray
				//GL_UNSIGNED_INT_SAMPLER_BUFFER	usamplerBuffer
				//GL_UNSIGNED_INT_SAMPLER_2D_RECT	usampler2DRect
				//GL_IMAGE_1D	image1D
				//GL_IMAGE_2D	image2D
				//GL_IMAGE_3D	image3D
				//GL_IMAGE_2D_RECT	image2DRect
				//GL_IMAGE_CUBE	imageCube
				//GL_IMAGE_BUFFER	imageBuffer
				//GL_IMAGE_1D_ARRAY	image1DArray
				//GL_IMAGE_2D_ARRAY	image2DArray
				//GL_IMAGE_2D_MULTISAMPLE	image2DMS
				//GL_IMAGE_2D_MULTISAMPLE_ARRAY	image2DMSArray
				//GL_INT_IMAGE_1D	iimage1D
				//GL_INT_IMAGE_2D	iimage2D
				//GL_INT_IMAGE_3D	iimage3D
				//GL_INT_IMAGE_2D_RECT	iimage2DRect
				//GL_INT_IMAGE_CUBE	iimageCube
				//GL_INT_IMAGE_BUFFER	iimageBuffer
				//GL_INT_IMAGE_1D_ARRAY	iimage1DArray
				//GL_INT_IMAGE_2D_ARRAY	iimage2DArray
				//GL_INT_IMAGE_2D_MULTISAMPLE	iimage2DMS
				//GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY	iimage2DMSArray
				//GL_UNSIGNED_INT_IMAGE_1D	uimage1D
				//GL_UNSIGNED_INT_IMAGE_2D	uimage2D
				//GL_UNSIGNED_INT_IMAGE_3D	uimage3D
				//GL_UNSIGNED_INT_IMAGE_2D_RECT	uimage2DRect
				//GL_UNSIGNED_INT_IMAGE_CUBE	uimageCube
				//GL_UNSIGNED_INT_IMAGE_BUFFER	uimageBuffer
				//GL_UNSIGNED_INT_IMAGE_1D_ARRAY	uimage1DArray
				//GL_UNSIGNED_INT_IMAGE_2D_ARRAY	uimage2DArray
				//GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE	uimage2DMS
				//GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY	uimage2DMSArray
				//GL_UNSIGNED_INT_ATOMIC_COUNTER	atomic_uint
		}
	}

	std::string loadShaderSource( const fs::path filepath )
	{
		std::string source;

		std::ifstream file( filepath.c_str() );
		if (!file.is_open())
		{
			std::ostringstream msg;
			std::cout << "could not open shader source file: " << filepath << ", are you sure the file exists?" << std::endl;
			return source;
		}

		while (!file.eof())
		{
			std::string line;
			getline( file, line );
			source.append( line );
			source.append( "\n" );
		}

		file.close();

		return source;
	}

	struct TmpProgramBinding
	{
		TmpProgramBinding( std::shared_ptr< GlslProgram > t, std::shared_ptr< GlslProgram > o ) : tmpProg( t ), oldProg( o )
		{
			if ( oldProg != tmpProg )
				GlslProgram::setActiveProgram( tmpProg );
		}

		~TmpProgramBinding()
		{
			if ( oldProg != tmpProg )
				GlslProgram::setActiveProgram( oldProg );
		}

		std::shared_ptr< GlslProgram > tmpProg;
		std::shared_ptr< GlslProgram > oldProg;
	};

	std::weak_ptr< GlslProgram > GlslProgram::sCurrentProgram = std::weak_ptr< GlslProgram >();

	void GlslProgram::setActiveProgram( std::shared_ptr< GlslProgram > p )
	{
		sCurrentProgram = p;
		if ( p.get() )	glUseProgram( p->mHandle );
		else			glUseProgram( 0 );
	}

	std::shared_ptr< GlslProgram > GlslProgram::create( std::vector< ShaderInfo > shaders, const bool printLog )
	{
		std::shared_ptr< GlslProgram > result;// (new GlslProgram, GlslProgram::Deleter());

		std::shared_ptr< GlslProgram > p( new GlslProgram, GlslProgram::Deleter() );
		p->mHandle = glCreateProgram();

		bool ok = true;
		for ( ShaderInfo const& s : shaders )
		{
			ok &= p->addShader( s.type, s.source );
		}
		ok &= p->link();

		if ( !ok )
		{
			std::cout << "--- program error log ---" << std::endl;
			std::cout << p->getProgramLog() << std::endl;
			std::string output = "One of your GLSL Programs could not be created, please check the console for an error log.";
			std::wstring stemp = std::wstring(output.begin(), output.end());
			LPCWSTR sw = stemp.c_str();
			int msgboxID = MessageBox(
				NULL,
				sw,
				(LPCWSTR)L"Yarr, your GLSL program could not be created",
				MB_ICONWARNING | MB_OK
			);
		}
		else
		{
			if ( printLog )
			{
				std::cout << "--- program attrib & uniform info ---" << std::endl;
				p->printActiveUniforms();
				p->printActiveAttributes();
			}
			result = p;
		}

		return result;
	}

	std::shared_ptr< GlslProgram > GlslProgram::create()
	{
		std::shared_ptr< GlslProgram > p( new GlslProgram, GlslProgram::Deleter() );
		p->mHandle = glCreateProgram();
		return p;
	}

	void GlslProgram::Deleter::operator()( GlslProgram *& p ) const
	{
		if ( p == nullptr )
			return;

		glDeleteProgram( p->mHandle );

		delete p;
		p = nullptr;
	}

	bool GlslProgram::addShader( const ShaderType shaderType, const std::string shaderSource )
	{
		GLenum type;
		switch ( shaderType )
		{
		case ShaderType::COMPUTE_SHADER:
			type = GL_COMPUTE_SHADER;
			break;
		case ShaderType::VERTEX_SHADER:
			type = GL_VERTEX_SHADER;
			break;
		case ShaderType::FRAGMENT_SHADER:
			type = GL_FRAGMENT_SHADER;
			break;
		case ShaderType::GEOMETRY_SHADER:
			type = GL_GEOMETRY_SHADER;
			break;
		case ShaderType::TESSELLATION_CONTROL_SHADER:
			type = GL_TESS_CONTROL_SHADER;
			break;
		case ShaderType::TESSELLATION_EVALUATION_SHADER:
			type = GL_TESS_EVALUATION_SHADER;
			break;
		default:
			std::cout << "unknown shader type, was ignored " << std::endl;
			return false;
		}

		GLuint handle = glCreateShader( type );

		const char* source = shaderSource.c_str();
		glShaderSource( handle, 1, &source, nullptr );
		glCompileShader( handle );

		GLint compileStatus;
		glGetShaderiv( handle, GL_COMPILE_STATUS, &compileStatus );

		GLint sz = 0;
		glGetShaderiv( handle, GL_INFO_LOG_LENGTH, &sz );
		if (sz > 0)
		{
			char *buffer = new char[ sz ];
			memset( buffer, 0, sz );
			glGetShaderInfoLog( handle, sz, NULL, buffer );
			mProgramLog.append( buffer );
			delete[] buffer;
		}

		if (!compileStatus)
			return false;

		glAttachShader( mHandle, handle );
		return true;
	}

	bool GlslProgram::link()
	{
		glLinkProgram( mHandle );

		GLint linkStatus;
		glGetProgramiv( mHandle, GL_LINK_STATUS, &linkStatus );

		GLint sz = 0;
		glGetProgramiv( mHandle, GL_INFO_LOG_LENGTH, &sz );
		if (sz > 0)
		{
			char *buffer = new char[ sz ];
			memset( buffer, 0, sz );
			glGetProgramInfoLog( mHandle, sz, NULL, buffer );
			mProgramLog.append( buffer );
			delete[] buffer;
		}

		if (!linkStatus)
			return false;

		queryActiveAttributes();
		queryActiveUniforms();
		return true;
	}

	bool GlslProgram::isValid() const
	{
		glValidateProgram( mHandle );
		GLint validateStatus;
		glGetProgramiv( mHandle, GL_VALIDATE_STATUS, &validateStatus );
		return (validateStatus ? true : false);
	}

	std::string GlslProgram::getProgramLog() const
	{
		return mProgramLog;
	}

	void GlslProgram::recordInterleavedOutputs( const std::vector< std::string > outputs )
	{
		std::vector< char const* > tmp;	//ugh
		for (unsigned int i = 0; i<outputs.size(); ++i) tmp.push_back( outputs[ i ].c_str() );
		glTransformFeedbackVaryings( mHandle, tmp.size(), &tmp[ 0 ], GL_INTERLEAVED_ATTRIBS );
	}

	void GlslProgram::queryActiveUniforms()
	{
		GLint count, maxLength;
		glGetProgramiv( mHandle, GL_ACTIVE_UNIFORMS, &count );
		glGetProgramiv( mHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength );

		char* buffer = new char[ maxLength ];
		memset( buffer, 0, maxLength );

		GLenum type;
		GLint size, location;
		std::string name;

		for (int i = 0; i < count; ++i)
		{
			glGetActiveUniform( mHandle, i, maxLength, nullptr, &size, &type, buffer );
			name = buffer;

			location = glGetUniformLocation( mHandle, buffer );
			VariableInfo info;
			info.location = location;
			info.size = size;
			info.type = type;
			mUniforms[ name ] = info;
		}

		delete[] buffer;
	}

	void GlslProgram::queryActiveAttributes()
	{
		GLint count, maxLength;
		glGetProgramiv( mHandle, GL_ACTIVE_ATTRIBUTES, &count );
		glGetProgramiv( mHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength );

		char* buffer = new char[ maxLength ];
		memset( buffer, 0, maxLength );

		GLint size, location;
		GLenum type;
		std::string name;

		for (int i = 0; i < count; ++i)
		{
			glGetActiveAttrib( mHandle, i, maxLength, nullptr, &size, &type, buffer );
			name = buffer;
			location = glGetAttribLocation( mHandle, buffer );
			VariableInfo info;
			info.location = location;
			info.size = size;
			info.type = type;
			mAttributes[ name ] = info;
		}

		delete[] buffer;
	}

	void GlslProgram::printActiveUniforms() const
	{
		for (auto it = mUniforms.begin(); it != mUniforms.end(); ++it)
			std::cout << "active uniform\t\t" << it->first << " at ( uniform ) location " << it->second.location << " (" << toGLSLType(it->second.type) << ")" << std::endl;
	}

	void GlslProgram::printActiveAttributes() const
	{
		for (auto it = mAttributes.begin(); it != mAttributes.end(); ++it)
			std::cout << "active attribute\t" << it->first << " at ( attribute ) location " << it->second.location << " (" << toGLSLType(it->second.type) << ")" << std::endl;
	}

	void GlslProgram::setUniformTexVal( const std::string name, const unsigned int texUnit )
	{
		GLint location = checkUniformInternal(name, { GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE } );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform1i( location, texUnit );
	}

	void GlslProgram::setUniformVal( const std::string name, const float v )
	{
		GLint location = checkUniformInternal( name, GL_FLOAT );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform1f( location, v );
	}

	void GlslProgram::setUniformBVal(const std::string name, const bool v)
	{
		GLint location = checkUniformInternal(name, GL_BOOL);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform1i(location, v);
	}

	void GlslProgram::setUniformIVal( const std::string name, const int v )
	{
		GLint location = checkUniformInternal( name, GL_INT );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform1i( location, v );
	}

	void GlslProgram::setUniformUVal( const std::string name, const unsigned int v )
	{
		GLint location = checkUniformInternal( name, GL_UNSIGNED_INT );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform1ui( location, v );
	}

	void GlslProgram::setUniformVec2(const std::string name, const glm::vec2 v)
	{
		GLint location = checkUniformInternal(name, GL_FLOAT_VEC2 );
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform2f(location, v.x, v.y);
	}

	void GlslProgram::setUniformVec3(const std::string name, const glm::vec3 v)
	{
		GLint location = checkUniformInternal(name, GL_FLOAT_VEC3);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform3f(location, v.x, v.y, v.z);
	}

	void GlslProgram::setUniformVec4(const std::string name, const glm::vec4 v)
	{
		GLint location = checkUniformInternal(name, GL_FLOAT_VEC4);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform4f(location, v.x, v.y, v.z, v.w);
	}

	void GlslProgram::setUniformVec2( const std::string name, const float x, const float y )
	{
		GLint location = checkUniformInternal( name, GL_FLOAT_VEC2 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform2f( location, x, y );
	}

	void GlslProgram::setUniformVec3( const std::string name, const float x, const float y, const float z )
	{
		GLint location = checkUniformInternal( name, GL_FLOAT_VEC3 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform3f( location, x, y, z );
	}

	void GlslProgram::setUniformVec4( const std::string name, const float x, const float y, const float z, const float w )
	{
		GLint location = checkUniformInternal( name, GL_FLOAT_VEC4 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform4f( location, x, y, z, w );
	}

	void GlslProgram::setUniformIVec2(const std::string name, const glm::ivec2 v)
	{
		GLint location = checkUniformInternal(name, GL_INT_VEC2 );
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform2i(location, v.x, v.y);
	}

	void GlslProgram::setUniformIVec3(const std::string name, const glm::ivec3 v)
	{
		GLint location = checkUniformInternal(name, GL_INT_VEC3);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform3i(location, v.x, v.y, v.z);
	}

	void GlslProgram::setUniformIVec4(const std::string name, const glm::ivec4 v)
	{
		GLint location = checkUniformInternal(name, GL_INT_VEC4);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform4i(location, v.x, v.y, v.z, v.w);
	}

	void GlslProgram::setUniformIVec2( const std::string name, const int x, const int y )
	{
		GLint location = checkUniformInternal( name, GL_INT_VEC2 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform2i( location, x, y );
	}

	void GlslProgram::setUniformIVec3( const std::string name, const int x, const int y, const int z )
	{
		GLint location = checkUniformInternal( name, GL_INT_VEC3 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform3i( location, x, y, z );
	}

	void GlslProgram::setUniformIVec4( const std::string name, const int x, const int y, const int z, const int w )
	{
		GLint location = checkUniformInternal( name, GL_INT_VEC4 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform4i( location, x, y, z, w );
	}

	void GlslProgram::setUniformUVec2(const std::string name, const glm::uvec2 v)
	{
		GLint location = checkUniformInternal(name, GL_UNSIGNED_INT_VEC2 );
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform2ui(location, v.x, v.y);
	}

	void GlslProgram::setUniformUVec3(const std::string name, const glm::uvec3 v)
	{
		GLint location = checkUniformInternal(name, GL_UNSIGNED_INT_VEC3);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform3ui(location, v.x, v.y, v.z);
	}

	void GlslProgram::setUniformUVec4(const std::string name, const glm::uvec4 v)
	{
		GLint location = checkUniformInternal(name, GL_UNSIGNED_INT_VEC4);
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());
		glUniform4ui(location, v.x, v.y, v.z, v.w);
	}

	void GlslProgram::setUniformUVec2( const std::string name, const unsigned int x, const unsigned int y )
	{
		GLint location = checkUniformInternal( name, GL_UNSIGNED_INT_VEC2);
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform2ui( location, x, y );
	}

	void GlslProgram::setUniformUVec3( const std::string name, const unsigned int x, const unsigned int y, const unsigned int z )
	{
		GLint location = checkUniformInternal( name, GL_UNSIGNED_INT_VEC3);
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform3ui( location, x, y, z );
	}

	void GlslProgram::setUniformUVec4( const std::string name, const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int w )
	{
		GLint location = checkUniformInternal( name, GL_UNSIGNED_INT_VEC4);
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniform4ui( location, x, y, z, w );
	}

	void GlslProgram::setUniformMat2( const std::string name, const glm::mat2 mat, const bool transpose )
	{
		GLint location = checkUniformInternal( name, GL_FLOAT_MAT2 );
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniformMatrix2fv( location, 1, transpose, std::addressof( mat[ 0 ][ 0 ] ) );
	}

	void GlslProgram::setUniformMat3( const std::string name, const glm::mat3 mat, const bool transpose )
	{
		GLint location = checkUniformInternal( name, GL_FLOAT_MAT3);
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniformMatrix3fv( location, 1, transpose, std::addressof( mat[ 0 ][ 0 ] ) );
	}

	void GlslProgram::setUniformMat4( const std::string name, const glm::mat4 mat, const bool transpose )
	{
		GLint location = checkUniformInternal( name , GL_FLOAT_MAT4);
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );
		glUniformMatrix4fv( location, 1, transpose,  std::addressof( mat[ 0 ][ 0 ] ) );
	}

	void GlslProgram::setUniformMat4Array( const std::string name, const std::vector< glm::mat4 > mats, const bool transpose )
	{
		TmpProgramBinding tmp( shared_from_this(), sCurrentProgram.lock() );

		std::stringstream base;
		base << name << "[" << 0 << "]";
		GLint location = checkUniformInternal( base.str(), GL_FLOAT_MAT4);

		for ( unsigned int i=0; i<mats.size(); ++i )
			glUniformMatrix4fv( location+i, 1, GL_FALSE, std::addressof( mats[ i ][ 0 ][ 0 ] ) );
	}

	void GlslProgram::setUniformVec3Array( const std::string name, const std::vector< glm::vec3 > val )
	{
		TmpProgramBinding tmp(shared_from_this(), sCurrentProgram.lock());

		std::stringstream base;
		base << name << "[" << 0 << "]";
		GLint location = checkUniformInternal(base.str(), GL_FLOAT_VEC3);
		if ( location == -1 )
		{
			std::cout << "-----------------------------------------------\n";
			std::cout << "the uniform array " << name << " was not found!\n";
			std::cout << "-----------------------------------------------\n";
		}

		glUniform3fv( location, val.size(), (float*)val.data() );
	}

	GLint GlslProgram::checkUniformInternal( const std::string name, const GLenum type )
	{
		GLint location = getUniformLocation( name );
		GLenum queriedType = getUniformType( name );
		bool wasWarned = getWarned(name);
		if (location == -1 && !wasWarned)
		{
			setWarned(name);
			std::string output = "Uniform " + name + " was not found...\nDoublecheck the spelling!\n(Spelled correctly but UNUSED by the shader will also trigger this message)";
			std::wstring stemp = std::wstring(output.begin(), output.end());
			LPCWSTR sw = stemp.c_str();
			int msgboxID = MessageBox(
				NULL,
				sw,
				(LPCWSTR)L"Yarr, you missspelled a uniform",
				MB_ICONWARNING | MB_OK
			);
		}
		else if (type != queriedType && !wasWarned)
		{
			setWarned(name);
			std::string output = "Uniform " + name + " has wrong datatype: " + toGLSLType(queriedType) + "...\n(You attempted to set " + toGLSLType(type) + ")";
			std::wstring stemp = std::wstring(output.begin(), output.end());
			LPCWSTR sw = stemp.c_str();
			int msgboxID = MessageBox(
				NULL,
				sw,
				(LPCWSTR)L"Yarr, you misstyped a uniform",
				MB_ICONWARNING | MB_OK
			);
		}
		return location;
	}

	GLint GlslProgram::checkUniformInternal(const std::string name, std::vector< GLenum > types)
	{
		GLint location = getUniformLocation(name);
		GLenum queriedType = getUniformType(name);
		bool containsType = false;
		for (auto t : types)
		{
			containsType |= (t == queriedType);
		}
		bool wasWarned = getWarned(name);
		if (location == -1 && !wasWarned)
		{
			setWarned(name);
			std::string output = "Uniform " + name + " was not found...\nDoublecheck the spelling!\n(Spelled correctly but UNUSED by the shader will also trigger this message)";
			std::wstring stemp = std::wstring(output.begin(), output.end());
			LPCWSTR sw = stemp.c_str();
			int msgboxID = MessageBox(
				NULL,
				sw,
				(LPCWSTR)L"Yarr, you missspelled a uniform",
				MB_ICONWARNING | MB_OK
			);
		}
		else if (!containsType && !wasWarned)
		{
			setWarned(name);
			std::string output = "Uniform " + name + " has wrong datatype: " + toGLSLType(queriedType) + "...\n";
			std::wstring stemp = std::wstring(output.begin(), output.end());
			LPCWSTR sw = stemp.c_str();
			int msgboxID = MessageBox(
				NULL,
				sw,
				(LPCWSTR)L"Yarr, you misstyped a uniform",
				MB_ICONWARNING | MB_OK
			);
		}
		return location;
	}

	GLint GlslProgram::getUniformLocation( const std::string name ) const
	{
		GLint location = -1;
		auto it = mUniforms.find( name );
		if ( it != mUniforms.end() )
			location = it->second.location;

		return location;
	}


	GLenum GlslProgram::getUniformType(const std::string name) const
	{
		GLenum type = -1;
		auto it = mUniforms.find(name);
		if (it != mUniforms.end())
			type = it->second.type;

		return type;
	}


	bool GlslProgram::getWarned(const std::string name) const
	{
		bool res = false;
		auto it = mWarnings.find(name);
		if (it != mWarnings.end())
			res = it->second;

		return res;
	}

	void GlslProgram::setWarned(const std::string name)
	{
		mWarnings[name] = true;
	}

	GLint GlslProgram::getFragDataLocation( const std::string name ) const
	{
		return glGetFragDataLocation( mHandle, name.c_str() );
	}

	GLint GlslProgram::getAttributeLocation( const std::string name ) const
	{
		GLint location = -1;
		auto it = mAttributes.find( name );
		if ( it != mAttributes.end() )
			location = it->second.location;

		return location;
	}
}