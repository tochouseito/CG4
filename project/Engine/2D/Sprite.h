#pragma once
#include"Mesh.h"
#include"Material.h"
#include"ViewProjection.h"
// 定数バッファ用データ構造体
struct ConstBufferDataSpriteWorldTransform {
	Matrix4x4 matWorld; // ローカル → ワールド変換行列
};
class Sprite
{
public:
	Sprite();
	~Sprite();
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, ViewProjection* viewProjection, std::string textureHandle);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 頂点リソースを作る
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// Sprite用のTransformationMatrix用のリソースを作る。
	/// </summary>
	void TransformResource();

	void UpdataTransform();

	void CreateMaterial();

	void CreateIndexResource();

	void uvTransformUpdata();
private:
	// Sprite用の頂点リソース
	Microsoft::WRL::ComPtr< ID3D12Resource> vertexResourceSprite_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_{};
	Mesh::VertexData* vertexDataSprite_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12Resource> transformationMatrixResourceSprite_;
	//ViewProjection::wvpData* transformationMatrixDataSprite_ = nullptr;
	ConstBufferDataSpriteWorldTransform* transformationMatrixDataSprite_ = nullptr;
	ViewProjection* viewProjection_;
	//WorldTransform worldTransform_;
	//WorldTransform uvTransformSprite_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	Microsoft::WRL::ComPtr < ID3D12Resource> materialResourceSprite_;
	Material::MaterialData* materialDataSprite_ = { nullptr };
	Microsoft::WRL::ComPtr < ID3D12Resource>indexResourceSprite_;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
	uint32_t* indexDataSprite_ = nullptr;
	std::string textureHandle_;

	Mesh* mesh_ = nullptr;
	Material* material_ = nullptr;

	// ローカルスケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_ = { 0 };

	// ローカルスケール
	Vector3 UVscale_ = { 1.0f, 1.0f, 1.0f };
	// X,Y,Z軸回りのローカル回転角
	Vector3 UVrotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 UVtranslation_ = { 0.0f, 0.0f, 0.0f };
};

