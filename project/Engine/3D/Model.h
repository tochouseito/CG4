#pragma once
#include "Mesh.h"
#include"Material.h"
#include"DirectionalLight.h"
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"PointLight.h"
#include"SpotLight.h"
#include <cstdint>
#include<random>
#include<unordered_map>

// external
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
class Model
{
public:
	Model();
	~Model();
	struct Node {
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
	};
	struct OBJModelData {
		std::vector < Mesh::VertexData>vertices;
		Material::OBJMaterialData material;
		Node rootNode;
	};
	struct ObjectData {
		std::vector < Mesh::VertexData>vertices;
		Material::OBJMaterialData material;
		bool useTexture = false;
	};
	struct ModelData {
		std::unordered_map<std::string, ObjectData> object;
		std::vector<std::string> names;
		Node rootNode;
	};
public:
	
	

	/// <summary>
	/// 描画
	/// </summary>
	/// テクスチャ上書き用
	void Draw(WorldTransform& worldTransform,ViewProjection& viewProjection,
		std::string textureHandle );

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(WorldTransform& worldTransform, ViewProjection& viewProjection);

	void SetTextureHandle(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU, uint32_t handleNum) { textureSrvHandleGPU_[handleNum] = textureSrvHandleGPU; }

	OBJModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	static ModelData* LoadModelFile(const std::string& directory, const std::string& filename);

	static Node ReadNode(aiNode* node);

	/// <summary>
	/// MTLの読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	Material::OBJMaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	static Model* LordModel(const std::string& filename);


	/*デフォルトオブジェクトの生成*/

	/*平面オブジェクト*/
	static Model* CreatePlane();

	/*球オブジェクト*/
	static Model* CreateSphere(uint32_t Subdivision = 16);

public:/*getter*/
	Material* GetMaterial() { return material_; }
	Mesh* GetMesh() { return mesh_; }
	PointLight* GetPointLight() { return pointLight_; }
	SpotLight* GetSpotLight() { return spotLight_; }
	DirectionalLight* GetDirectionalLight() { return directionalLight_; }
public:/*setter*/
	void SetBlendMode(uint32_t blendMode) { current_blend = blendMode; }

private:/*メンバ変数*/
	
private:
	ViewProjection* viewProjection_;
	WorldTransform worldTransform_;
	Material* material_;
	Mesh* mesh_;
	PointLight* pointLight_;
	SpotLight* spotLight_;
	DirectionalLight* directionalLight_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_[2];
	bool useMonsterBall = true;
	//OBJModelData modelData_;
	std::string textureHandle_;
	//std::unordered_map<std::string, ObjectData> modelData_;
	//std::unordered_map<std::string, OBJModelData> modelData_;
	ModelData* modelData_;
	// 現在選択されているアイテムのインデックス
	uint32_t current_blend = 0;
	//UINT vertices = 0;

	
};

