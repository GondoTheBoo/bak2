#pragma once

#include "glm.h"
#include "glm_additional.h"
#include "opengl.h"

#include <map>
#include <vector>
#include <memory>
#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;

namespace cg2
{
	class Texture2D;

	enum class Attrib : unsigned char { POSITION, NORMAL, TANGENT, BITANGENT, TEXCOORD };
	typedef unsigned int AttribLocation;
	typedef std::map< Attrib, AttribLocation > AttribBindings;

	struct VertexData
	{
		glm::vec3					mPosition;
		glm::vec3					mNormal;
		glm::vec3					mTangent;
		glm::vec3					mBitangent;
		glm::vec2					mTexcoord;
	};

	struct BasicMaterial
	{
		glm::vec3 mKDiffuse;
		glm::vec3 mKSpecular;
		glm::vec3 mKAmbient;
		float mShininess;
		float mOpacity;
		float mRefractivity;
		fs::path mDiffuseTexture;
		fs::path mSpecularTexture;
		fs::path mAOTexture;
		fs::path mNormalMap;
	};

	struct AABB
	{
		glm::vec3 mUpperBound;
		glm::vec3 mLowerBound;
	};

	struct TrimeshData
	{
		unsigned					mVertexCount = 0;
		unsigned					mIndexCount = 0;
		BasicMaterial				mMaterial;
		AABB						mAABB;
	};

	struct TrimeshDataGPU
	{
		GLuint						mHandleToVertexBuffer = 0;
		GLuint						mHandleToIndexBuffer = 0;
	};

	// vertex array objects are separated form the rest ( -> buildVertexArrayObjects )
	// this was done because at the time, I needed to functionality to render the same asset
	// with a varienty of different shaders, with different attribute bindings

	class TriAsset
	{

	public:

		TriAsset(TriAsset const& other) = delete;
		TriAsset& operator = (TriAsset const& other) = delete;

		// imports asset, uploads meshes & textures to gpu and builds default vertex array objects
		// meshes must provide position data (duh) as well as normals. uvs and tangent space vectors
		// are optional and will be initialized to default values if missing
		static std::shared_ptr< const TriAsset > import(const fs::path path);

		// may be used to build custom vertex array objects
		std::vector< GLuint > buildVertexArrayObjects(const AttribBindings bindings = { { Attrib::POSITION, 0 },
																						{ Attrib::NORMAL, 1 },
																						{ Attrib::TANGENT, 2 },
																						{ Attrib::BITANGENT, 3 },
																						{ Attrib::TEXCOORD, 4 } }) const;

		bool													mHasTangentSpace;
		bool													mHasUVs;

		struct Node
		{
			std::string					mName;
			glm::mat4					mOffsetTf;				// model tf relative to parent node
			glm::mat4					mModelTf;				// absolute model tf
			std::vector< unsigned >		mMeshIds;				// all meshes referenced by this node --> can be used to index mVAOs, mMeshDataCPU, mMeshDataGPU, mMaterials
			Node						*mParent = nullptr;
			std::vector< Node * >		mChildren;
		};

		std::vector< Node * >									mScenegraph;	// level-order traversal

		std::map< fs::path, std::shared_ptr< Texture2D > >		mTextures;

		std::vector< unsigned >									mVAOs;			// vertex array objects, using default attrib bindings
		std::vector< TrimeshDataGPU >							mMeshDataGPU;	// mesh data @ gpu
		std::vector< TrimeshData >								mMeshDataCPU;	// mesh data @ cpu
		std::vector< BasicMaterial >							mMaterials;

	private:

		TriAsset() = default;
		~TriAsset() = default;

		struct Deleter
		{
			void operator()(TriAsset *& p) const;
		};

	};
}