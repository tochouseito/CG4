#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include"Vector4.h"
#include<d3d12.h>
#include<wrl.h>
#include <cstdint>
#include<vector>
#include<unordered_map>

class Model;
class Mesh  {
public:
	Mesh();
	~Mesh() = default;
private:// メンバ関数
	

	
public: 
	// 頂点データ構造体（テクスチャあり）
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	enum class Object {
		kBox,
		kPlane,
		kSphere,
	};
	struct MeshData {
		VertexData* vertexData;
		Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		UINT vertices = 0;
		uint32_t* indexData = nullptr;
		Microsoft::WRL::ComPtr < ID3D12Resource> indexResource;
		D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	};
public: // メンバ関数
	
	/// <summary>
	/// 頂点リソースを作る
	/// </summary>
	void CreateVertexResource(size_t vertices);

	void CreateParticleVertexResource(size_t vertices);

	void CreateGSVertexResource(size_t vertices);

	void CreateSpriteVertexResource(size_t vertices);

	void CreateSpriteIndexResource();

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }

	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferViewSphere_; }

	uint32_t GetBallVertex() { return ballVertex_; }

	void CreateIndexResource();

	VertexData* GetVertexData() { return vertexData_; }

	/*メッシュデータのGetter*/
	VertexData* GetData(const std::string& objectName) {return meshData_[objectName].vertexData; }
	MeshData GetMeshData(const std::string& objectName) { return meshData_[objectName]; }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView(const std::string& objectName) {
		return &meshData_[objectName].indexBufferView;
	}
	void SetDataVertices(UINT vertices, const std::string& name) { meshData_[name].vertices = vertices; }
	void CreateDateResource(size_t vertices, const std::string& name);
	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView(const std::string& name) {
		return &meshData_[name].vertexBufferView; 
	}
	D3D12_VERTEX_BUFFER_VIEW GetVBV(const std::string& name) {
		return meshData_[name].vertexBufferView;
	}
	UINT GetDataVertices(const std::string& name) {
		
		return meshData_[name].vertices;
	}

	void CreateBall(uint32_t Subdivision);

	/*デフォルトオブジェクトの頂点リソースを作る*/
	void CreateObjectVertex(Object Object);

	void CreateModelIndexResource(size_t indices, const std::string& name);

	void CreateSkyBoxVertexResource();

	/// <summary>
	/// 頂点リソースを作る
	/// </summary>
	void CreateOBJVertexResource(size_t vertices);
	void SetVertices(UINT vertices) { vertices_ = vertices; }
	UINT GetVertices() { return vertices_; }
private: // メンバ変数
	
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource_;
	VertexData* vertexData_ = nullptr;
	std::unordered_map <std::string, MeshData> meshData_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	uint32_t kSubdivision_=0;
	float kLonEvery_=0.0f; // 経度分割1つ分の角度
	float kLatEvery_ = 0.0f;        // 緯度分割1つ分の角度
	uint32_t ballVertex_ = 0;
	Microsoft::WRL::ComPtr < ID3D12Resource> indexResourceSphere_;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere_{};
	uint32_t* indexDataSphere_ = nullptr;
	Object object_ = Object::kBox;
	UINT vertices_ = 0;
	uint32_t* indexData_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t boxVertex_ = 24;
};

