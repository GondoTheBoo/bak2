#include "triasset.h"
#include "texture2D.h"

#include "assimp.h"
#include "assimp_and_glm.h"

#include <deque>

namespace cg2
{

	struct TrimeshDataCPU
	{
		std::vector< VertexData >	mVertexData;
		std::vector< unsigned >		mIndexData;
		unsigned					mVertexCount = 0;
		unsigned					mIndexCount = 0;
		AABB						mAABB;
	};

	TriAsset::Node * createNode(aiNode *ainode)
	{
		if (ainode == nullptr)
			return nullptr;

		TriAsset::Node *result = new TriAsset::Node;
		result->mName = ainode->mName.C_Str();
		result->mOffsetTf = toGlm::mat4(ainode->mTransformation);
		return result;
	}

	TrimeshDataCPU buildInterleaved(aiMesh *aimesh)
	{
		TrimeshDataCPU result;

		if (aimesh == nullptr)
			return result;

		// this better be true... ( else there's really no point to this mesh )
		assert(aimesh->HasPositions() && aimesh->HasFaces());

		//glm::vec3 min = glm::vec3( std::numeric_limits<float>::max() );
		float minX = std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float minZ = std::numeric_limits<float>::max();

		//glm::vec3 max = glm::vec3( std::numeric_limits<float>::min() );
		float maxX = std::numeric_limits<float>::min();
		float maxY = std::numeric_limits<float>::min();
		float maxZ = std::numeric_limits<float>::min();

		result.mVertexData.reserve(aimesh->mNumVertices);
		for (unsigned v = 0; v < aimesh->mNumVertices; ++v)
		{
			cg2::VertexData vertex;
			vertex.mPosition = toGlm::vec3(aimesh->mVertices[v]);
			//min = glm::min(min, vertex.mPosition);
			//max = glm::max(max, vertex.mPosition);

			minX = glm::min(minX, vertex.mPosition.x);
			maxX = glm::max(maxX, vertex.mPosition.x);
			minY = glm::min(minY, vertex.mPosition.y);
			maxY = glm::max(maxY, vertex.mPosition.y);
			minZ = glm::min(minZ, vertex.mPosition.z);
			maxZ = glm::max(maxZ, vertex.mPosition.z);

			if (aimesh->HasNormals())
			{
				vertex.mNormal = glm::normalize(toGlm::vec3(aimesh->mNormals[v]));
			}
			else
			{
				vertex.mNormal = glm::vec3(0.f, 0.f, 1.f);
			}


			if (aimesh->HasTextureCoords(0))
			{
				vertex.mTexcoord = toGlm::vec2(aimesh->mTextureCoords[0][v]);
			}
			else
			{
				vertex.mTexcoord = glm::vec2(0.5f, 0.5f);
			}

			if (aimesh->HasTangentsAndBitangents())
			{
				vertex.mTangent = toGlm::vec3(aimesh->mTangents[v]);
				vertex.mBitangent = toGlm::vec3(aimesh->mBitangents[v]);
			}
			else
			{
				vertex.mTangent = glm::vec3(0.f, 1.f, 0.f);
				vertex.mBitangent = glm::vec3(1.f, 0.f, 0.f);
			}

			// perform gram-schmidt orthogonalization
			vertex.mTangent = vertex.mTangent - glm::dot(vertex.mTangent, vertex.mNormal) * vertex.mNormal;
			vertex.mBitangent = glm::cross(vertex.mTangent, vertex.mNormal);

			result.mVertexData.push_back(vertex);
		}
		result.mVertexCount = aimesh->mNumVertices;

		result.mIndexData.reserve(aimesh->mNumFaces * 3);
		for (unsigned f = 0; f < aimesh->mNumFaces; ++f)
		{
			// we assume triangle meshes
			assert(aimesh->mFaces[f].mNumIndices == 3);
			result.mIndexData.push_back(aimesh->mFaces[f].mIndices[0]);
			result.mIndexData.push_back(aimesh->mFaces[f].mIndices[1]);
			result.mIndexData.push_back(aimesh->mFaces[f].mIndices[2]);
		}
		result.mIndexCount = aimesh->mNumFaces * 3;

		result.mAABB.mLowerBound = glm::vec3(minX, minY, minZ);
		result.mAABB.mUpperBound = glm::vec3(maxX, maxY, maxZ);

		return result;
	}

	TrimeshDataGPU moveToGPU(const TrimeshDataCPU meshData)
	{
		TrimeshDataGPU result;

		glGenBuffers(1, std::addressof(result.mHandleToVertexBuffer));
		glGenBuffers(1, std::addressof(result.mHandleToIndexBuffer));

		glBindBuffer(GL_ARRAY_BUFFER, result.mHandleToVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * meshData.mVertexCount, std::addressof(meshData.mVertexData.front()), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.mHandleToIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) *  meshData.mIndexCount, std::addressof(meshData.mIndexData.front()), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return result;
	}

	std::shared_ptr< const TriAsset > TriAsset::import(const fs::path path)
	{
		fs::path directory = path; directory.remove_filename();

		std::shared_ptr< TriAsset > result(new TriAsset, TriAsset::Deleter());

		Assimp::Importer importer;
		aiScene const* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		if (!scene)
		{
			std::cout << "import error: asset file not found or corrupted " << path << std::endl;
			return nullptr;
		}

		std::cout << "imported asset @" << path << std::endl;
		std::cout << "the asset contains " << scene->mNumMeshes << " meshes" << std::endl;
		std::cout << "the asset contains " << scene->mNumMaterials << " materials" << std::endl;
		//std::cout << "the asset contains " << scene->mNumTextures << " textures" << std::endl;

		for (unsigned i = 0; i < scene->mNumMeshes; ++i)
		{
			auto mesh = scene->mMeshes[i];
			assert(mesh->HasFaces() && mesh->HasNormals() && mesh->HasPositions() && mesh->mMaterialIndex < scene->mNumMaterials);
		}

		for (unsigned i = 0; i < scene->mNumMaterials; ++i)
		{
			BasicMaterial material;
			auto aimaterial = scene->mMaterials[i];

			if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString texturePath;
				if (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
				{
					material.mDiffuseTexture = directory / texturePath.C_Str();
					if (result->mTextures.find(material.mDiffuseTexture) == result->mTextures.end())
					{
						result->mTextures[material.mDiffuseTexture] = Texture2D::import(material.mDiffuseTexture);
					}
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				aiString texturePath;
				if (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
				{
					material.mSpecularTexture = directory / texturePath.C_Str();
					if (result->mTextures.find(material.mSpecularTexture) == result->mTextures.end())
					{
						result->mTextures[material.mSpecularTexture] = Texture2D::import(material.mSpecularTexture);
					}
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				aiString texturePath;
				if (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &texturePath))
				{
					material.mAOTexture = directory / texturePath.C_Str();
					if (result->mTextures.find(material.mAOTexture) == result->mTextures.end())
					{
						result->mTextures[material.mAOTexture] = Texture2D::import(material.mAOTexture);
					}
				}
			}

			if (aimaterial->GetTextureCount(aiTextureType_HEIGHT) > 0)
			{
				aiString texturePath;
				if (AI_SUCCESS == scene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &texturePath))
				{
					material.mNormalMap = directory / texturePath.C_Str();
					if (result->mTextures.find(material.mNormalMap) == result->mTextures.end())
					{
						result->mTextures[material.mNormalMap] = Texture2D::import(material.mNormalMap);
					}
				}
			}

			aiColor4D diffuse, specular, ambient;
			if (AI_SUCCESS == aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_AMBIENT, &ambient))
				material.mKAmbient = toGlm::vec3(ambient);
			if (AI_SUCCESS == aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
				material.mKDiffuse = toGlm::vec3(diffuse);
			if (AI_SUCCESS == aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_SPECULAR, &specular))
				material.mKSpecular = toGlm::vec3(specular);

			float shininess, opacity, refractivity;
			if (AI_SUCCESS == aiGetMaterialFloat(aimaterial, AI_MATKEY_SHININESS, &shininess))
				material.mShininess = shininess;
			if (AI_SUCCESS == aiGetMaterialFloat(aimaterial, AI_MATKEY_OPACITY, &opacity))
				material.mOpacity = opacity;
			if (AI_SUCCESS == aiGetMaterialFloat(aimaterial, AI_MATKEY_REFRACTI, &refractivity))
				material.mRefractivity = refractivity;

			result->mMaterials.push_back(material);
		}

		/*
		*	node hierachy is stored in a vector, level-order ( breadth-first )
		*	the first element is, of course, the root node
		*/
		std::deque< std::pair< aiNode *, TriAsset::Node * > > tmp;
		tmp.push_back(std::make_pair(scene->mRootNode, nullptr));
		while (!tmp.empty())
		{
			auto current = tmp.front().first;
			auto parent = tmp.front().second;

			TriAsset::Node *node = createNode(current);
			node->mParent = parent;
			if (parent != nullptr)
				parent->mChildren.push_back(node);
			for (unsigned int m = 0; m < current->mNumMeshes; ++m)
				node->mMeshIds.push_back(current->mMeshes[m]);

			result->mScenegraph.push_back(node);

			for (unsigned int c = 0; c < current->mNumChildren; ++c)
			{
				aiNode *child = current->mChildren[c];
				tmp.push_back(std::make_pair(child, node));
			}

			tmp.pop_front();
		}

		/*
		*	scenegraph is static, and this class is not supposed to handle animations
		*	meaning, the total transformations of the nodes can be computed right now & never change
		*/
		for (auto node : result->mScenegraph)
		{
			if (node->mParent != nullptr)
				node->mModelTf = node->mParent->mModelTf * node->mOffsetTf;
			else
				node->mModelTf = node->mOffsetTf;
		}

		for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
		{
			aiMesh *aimesh = scene->mMeshes[m];
			auto tmpMesh = buildInterleaved(aimesh);
			TrimeshData meshData;
			meshData.mIndexCount = tmpMesh.mIndexCount;
			meshData.mVertexCount = tmpMesh.mVertexCount;
			meshData.mMaterial = result->mMaterials[aimesh->mMaterialIndex];
			meshData.mAABB = tmpMesh.mAABB;
			TrimeshDataGPU meshGPU = moveToGPU(tmpMesh);
			if (meshGPU.mHandleToIndexBuffer == 0 || meshGPU.mHandleToVertexBuffer == 0)
			{
				std::cout << "import error: something went wrong while building GPU meshes" << std::endl;
				importer.FreeScene();
				return nullptr;
			}

			result->mMeshDataCPU.push_back(meshData);
			result->mMeshDataGPU.push_back(meshGPU);
		}

		result->mVAOs = result->buildVertexArrayObjects();

		importer.FreeScene();
		return result;
	}

	std::vector< GLuint > TriAsset::buildVertexArrayObjects(const AttribBindings bindings) const
	{
		std::vector< GLuint > result;

		for (auto mesh : mMeshDataGPU)
		{
			GLuint handle = 0;
			glGenVertexArrays(1, std::addressof(handle));
			glBindVertexArray(handle);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.mHandleToVertexBuffer);

			for (auto binding : bindings)
			{
				glEnableVertexAttribArray(binding.second);
				switch (binding.first)
				{
				case Attrib::TEXCOORD:
					glVertexAttribPointer(binding.second, 2, GL_FLOAT, false, sizeof(VertexData), reinterpret_cast<GLvoid *const>(offsetof(VertexData, mTexcoord)));
					break;
				case Attrib::NORMAL:
					glVertexAttribPointer(binding.second, 3, GL_FLOAT, false, sizeof(VertexData), reinterpret_cast<GLvoid *const>(offsetof(VertexData, mNormal)));
					break;
				case Attrib::TANGENT:
					glVertexAttribPointer(binding.second, 3, GL_FLOAT, false, sizeof(VertexData), reinterpret_cast<GLvoid *const>(offsetof(VertexData, mTangent)));
					break;
				case Attrib::BITANGENT:
					glVertexAttribPointer(binding.second, 3, GL_FLOAT, false, sizeof(VertexData), reinterpret_cast<GLvoid *const>(offsetof(VertexData, mBitangent)));
					break;
				default:
					glVertexAttribPointer(binding.second, 3, GL_FLOAT, false, sizeof(VertexData), reinterpret_cast<GLvoid *const>(offsetof(VertexData, mPosition)));
					break;
				}
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mHandleToIndexBuffer);
			glBindVertexArray(0);
			result.push_back(handle);
		}

		return result;
	}

	void TriAsset::Deleter::operator()(TriAsset *& p) const
	{
		if (p == nullptr)
			return;

		for (auto mesh : p->mMeshDataGPU)
		{
			glDeleteBuffers(1, std::addressof(mesh.mHandleToVertexBuffer));
			glDeleteBuffers(1, std::addressof(mesh.mHandleToIndexBuffer));
		}

		for (auto node : p->mScenegraph)
		{
			delete node;
		}

		delete p;
		p = nullptr;
	}
}