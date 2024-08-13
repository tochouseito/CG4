#include "Mesh.h"
#include"Mymath.h"
#include"DirectXCommon.h"
#include"Model.h"
Mesh::Mesh() {
}
void Mesh::CreateDateResource(size_t vertices, const std::string& name)
{
	meshData_[name].vertexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertices);

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	meshData_[name].vertexBufferView.BufferLocation = meshData_[name].vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	meshData_[name].vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertices);
	// 1頂点アタリのサイズ
	meshData_[name].vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	meshData_[name].vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&meshData_[name].vertexData));
}
void Mesh::CreateBall(uint32_t Subdivision) {
	kSubdivision_ = Subdivision;   // 分割数
	kLonEvery_ = 2.0f * float(M_PI) / kSubdivision_; // 経度分割1つ分の角度
	kLatEvery_ = float(M_PI) / kSubdivision_;        // 緯度分割1つ分の角度
	ballVertex_ = (kSubdivision_ * kSubdivision_ * 6);
}
/// <summary>
/// 頂点リソースを作る
/// </summary>
void Mesh::CreateVertexResource(size_t vertices) {
	
	
	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertices);

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertices);
	// 1頂点アタリのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//std::memcpy(vertexData_, Model::GetModelData()->vertices.data(), sizeof(VertexData) * vertices);// 頂点データをリソースにコピー
	
	// 緯度の方向に分割　-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery_ * latIndex; // 現在の緯度

		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 6;//4;
			float lon = lonIndex * kLonEvery_; // 現在の経度
			uint32_t startIndex = (latIndex * kSubdivision_ + lonIndex) * 6;//4;
			float u = float(lonIndex) / float(kSubdivision_);
			float v = 1.0f - float(latIndex) / float(kSubdivision_);
			// 頂点にデータを入力する
			// A頂点
			vertexData_[start].position.x = cos(lat) * cos(lon);
			vertexData_[start].position.y = sin(lat);
			vertexData_[start].position.z = cos(lat) * sin(lon);
			vertexData_[start].position.w = 1.0f;
			vertexData_[start].texcoord = { float(lonIndex) / float(kSubdivision_),1.0f - float(latIndex) / float(kSubdivision_) };
			vertexData_[start].normal.x = vertexData_[start].position.x;
			vertexData_[start].normal.y = vertexData_[start].position.y;
			vertexData_[start].normal.z = vertexData_[start].position.z;
			// B頂点
			vertexData_[start + 1].position.x = cos(lat + kLatEvery_) * cos(lon);
			vertexData_[start + 1].position.y = sin(lat + kLatEvery_);
			vertexData_[start + 1].position.z = cos(lat + kLatEvery_) * sin(lon);
			vertexData_[start + 1].position.w = 1.0f;
			vertexData_[start + 1].texcoord = { float(lonIndex) / float(kSubdivision_),1.0f - float(latIndex + 1) / float(kSubdivision_) };
			vertexData_[start + 1].normal.x = vertexData_[start + 1].position.x;
			vertexData_[start + 1].normal.y = vertexData_[start + 1].position.y;
			vertexData_[start + 1].normal.z = vertexData_[start + 1].position.z;
			// C頂点
			vertexData_[start + 2].position.x = cos(lat) * cos(lon + kLonEvery_);
			vertexData_[start + 2].position.y = sin(lat);
			vertexData_[start + 2].position.z = cos(lat) * sin(lon + kLonEvery_);
			vertexData_[start + 2].position.w = 1.0f;
			vertexData_[start + 2].texcoord = { float(lonIndex + 1) / float(kSubdivision_),1.0f - float(latIndex) / float(kSubdivision_) };
			vertexData_[start + 2].normal.x = vertexData_[start + 2].position.x;
			vertexData_[start + 2].normal.y = vertexData_[start + 2].position.y;
			vertexData_[start + 2].normal.z = vertexData_[start + 2].position.z;
			// D頂点
			vertexData_[start + 3].position.x = cos(lat + kLatEvery_) * cos(lon + kLonEvery_);
			vertexData_[start + 3].position.y = sin(lat + kLatEvery_);
			vertexData_[start + 3].position.z = cos(lat + kLatEvery_) * sin(lon + kLonEvery_);
			vertexData_[start + 3].position.w = 1.0f;
			vertexData_[start + 3].texcoord = { float(lonIndex + 1) / float(kSubdivision_),1.0f - float(latIndex + 1) / float(kSubdivision_) };
			vertexData_[start + 3].normal.x = vertexData_[start + 3].position.x;
			vertexData_[start + 3].normal.y = vertexData_[start + 3].position.y;
			vertexData_[start + 3].normal.z = vertexData_[start + 3].position.z;

			// C頂点
			vertexData_[start + 4].position.x = cos(lat) * cos(lon + kLonEvery_);
			vertexData_[start + 4].position.y = sin(lat);
			vertexData_[start + 4].position.z = cos(lat) * sin(lon + kLonEvery_);
			vertexData_[start + 4].position.w = 1.0f;
			vertexData_[start + 4].texcoord = { float(lonIndex+1) / float(kSubdivision_),1.0f - float(latIndex) / float(kSubdivision_) };
			vertexData_[start + 4].normal.x = vertexData_[start + 4].position.x;
			vertexData_[start + 4].normal.y = vertexData_[start + 4].position.y;
			vertexData_[start + 4].normal.z = vertexData_[start + 4].position.z;
			// B頂点
			vertexData_[start + 5].position.x = cos(lat + kLatEvery_) * cos(lon);
			vertexData_[start + 5].position.y = sin(lat + kLatEvery_);
			vertexData_[start + 5].position.z = cos(lat + kLatEvery_) * sin(lon);
			vertexData_[start + 5].position.w = 1.0f;
			vertexData_[start + 5].texcoord = { float(lonIndex) / float(kSubdivision_),1.0f - float(latIndex+1) / float(kSubdivision_) };
			vertexData_[start + 5].normal.x = vertexData_[start + 5].position.x;
			vertexData_[start + 5].normal.y = vertexData_[start + 5].position.y;
			vertexData_[start + 5].normal.z = vertexData_[start + 5].position.z;

		}
	}
}

void Mesh::CreateParticleVertexResource(size_t vertices) {

	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertices);

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertices);
	// 1頂点アタリのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));


	vertexData_[0] = { {1.0f,1.0f,0.0f,1.0f},{0.0f,0.0f},{0.0f,0.0f,1.0f} };
	vertexData_[1] = { {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} };
	vertexData_[2] = { {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} };
	vertexData_[3] = { {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} };
	vertexData_[4] = { {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} };
	vertexData_[5] = { {-1.0f,-1.0f,0.0f,1.0f},{1.0f,1.0f},{0.0f,0.0f,1.0f} };
}

void Mesh::CreateGSVertexResource(size_t vertices) {
	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertices);
	vertices_ = static_cast<UINT>(vertices);
	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertices);
	// 1頂点アタリのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0] = { {1.0f,1.0f,0.0f,1.0f},{0.0f,0.0f},{0.0f,0.0f,1.0f} };
	/*vertexData_[1] = { {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} };
	vertexData_[2] = { {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} };
	vertexData_[3] = { {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} };
	vertexData_[4] = { {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} };
	vertexData_[5] = { {-1.0f,-1.0f,0.0f,1.0f},{1.0f,1.0f},{0.0f,0.0f,1.0f} };*/
}
/// <summary>
/// 頂点リソースを作る
/// </summary>
void Mesh::CreateOBJVertexResource(size_t vertices) {


	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertices);

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertices);
	// 1頂点アタリのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	

	
}
void Mesh::CreateIndexResource() {
	indexResourceSphere_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * uint32_t(ballVertex_));
	indexBufferViewSphere_.BufferLocation = indexResourceSphere_->GetGPUVirtualAddress();
	indexBufferViewSphere_.SizeInBytes = sizeof(uint32_t) * uint32_t(ballVertex_);
	indexBufferViewSphere_.Format = DXGI_FORMAT_R32_UINT;
	indexResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSphere_));
	for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 6;
			uint32_t startIndex = (latIndex * kSubdivision_ + lonIndex) * 4;
			indexDataSphere_[start] = 0 + startIndex; indexDataSphere_[start + 1] = 1 + startIndex; indexDataSphere_[start + 2] = 2 + startIndex;
			indexDataSphere_[start + 3] = 1 + startIndex; indexDataSphere_[start + 4] = 3 + startIndex; indexDataSphere_[start + 5] = 2 + startIndex;
		}
	}
}
/*デフォルトオブジェクトの頂点リソースを作る*/
void Mesh::CreateObjectVertex(Object Object) {
	object_ = Object;
	switch (object_)
	{
	case Object::kBox:
	default:

		break;
	case Object::kPlane:
		SetVertices(6);
		CreateParticleVertexResource(vertices_);
		break;
	case Object::kSphere:
		CreateBall(16);
		SetVertices(ballVertex_);
		CreateVertexResource(vertices_);
		break;
	}
}

void Mesh::CreateModelIndexResource(size_t indices, const std::string& name)
{
	meshData_[name].indexResource = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * indices);
	meshData_[name].indexBufferView.BufferLocation = meshData_[name].indexResource->GetGPUVirtualAddress();
	meshData_[name].indexBufferView.SizeInBytes = sizeof(uint32_t) * indices;
	meshData_[name].indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	meshData_[name].indexResource->Map(0, nullptr, reinterpret_cast<void**>(&meshData_[name].indexData));
}

void Mesh::CreateSpriteVertexResource(size_t vertices) {
	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * vertices);

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertices);
	// 1頂点アタリのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// １枚目の三角形
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };// 左下
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };// 左上
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };// 右下
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };// 左上
	vertexData_[3].texcoord = { 1.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[3].position = { 0.0f,0.0f,0.0f,1.0f };// 左下
	vertexData_[3].texcoord = { 0.0f,0.0f };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[4].position = { 640.0f,0.0f,0.0f,1.0f };// 左上
	vertexData_[4].texcoord = { 1.0f,0.0f };
	vertexData_[4].normal = { 0.0f,0.0f,-1.0f };
	vertexData_[5].position = { 640.0f,360.0f,0.0f,1.0f };// 右下
	vertexData_[5].texcoord = { 1.0f,1.0f };
	vertexData_[5].normal = { 0.0f,0.0f,-1.0f };
}