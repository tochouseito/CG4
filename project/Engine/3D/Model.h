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
#include"mathShapes.h"
#include<map>
#include<optional>

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
		WorldTransform::QuaternionTransform transform;
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
	//struct KeyframeVector3 {
	//	Vector3 value;//!<キーフレームの値
	//	float time;//!<キーフレームの時刻（単位は秒）
	//};
	//struct KeyframeQuaternion {
	//	Quaternion value;//!<キーフレームの値
	//	float time;//!<キーフレームの時刻（単位は秒）
	//};
	template<typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};
	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;
	template<typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};
	struct NodeAnimation {
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};
	struct Animation {
		float duration;// アニメーション全体の尺（単位は秒)
		// NodeAnimationの集合。Node名で引けるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
	};
	struct Joint {
		WorldTransform::QuaternionTransform transform;// Transform情報
		Matrix4x4 localMatrix;// localMatrix
		Matrix4x4 skeletonSpaceMatrix;// skeletonSpaceでの変換行列
		std::string name;// 名前
		std::vector<int32_t> children;// 子JointのIndexのリストいなければ空
		int32_t index; // 自身のIndex
		std::optional<int32_t> parent;// 親JointのIndex。いなければnull
	};
	struct Skeleton {
		int32_t root;// RootJointのIndex
		std::map<std::string, int32_t> jointMap;// Join名とIndexとの辞書
		std::vector<Joint> joints;// 所属しているジョイント
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

	static Animation* LordAnimationFile(const std::string& directoryPath, const std::string& filename);

	/*デフォルトオブジェクトの生成*/

	/*平面オブジェクト*/
	static Model* CreatePlane();

	/*球オブジェクト*/
	static Model* CreateSphere(uint32_t Subdivision = 16);

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	Skeleton CreateSkeleton(const Node& rootNode);

	int32_t CreateJoint(const Node& node,
		const std::optional<int32_t>& parent,
		std::vector<Joint>& joints);

	void SkeletonUpdata(Skeleton& skeleton);

	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

public:/*getter*/
	Material* GetMaterial() { return material_; }
	Mesh* GetMesh() { return mesh_; }
	PointLight* GetPointLight() { return pointLight_; }
	SpotLight* GetSpotLight() { return spotLight_; }
	DirectionalLight* GetDirectionalLight() { return directionalLight_; }
	Animation* GetAnimation() { 
		return animation_;
	}
	ModelData* GetModelData() { 
		return modelData_;
	}
public:/*setter*/
	void SetBlendMode(uint32_t blendMode) { current_blend = blendMode; }
	void SetAnimation(Animation* animation) { animation_ = animation; }
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
	Animation* animation_;
	// 現在選択されているアイテムのインデックス
	uint32_t current_blend = 0;
	//UINT vertices = 0;

	
};

