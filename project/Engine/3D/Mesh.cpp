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

void Mesh::CreateSkyBoxVertexResource()
{
	vertexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) *boxVertex_ );

	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(boxVertex_);
	// 1頂点アタリのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	/* 右面。描画インデックスは[0,1,2][2,1,3]で内側を向く */
	vertexData_[0].position = { 1.0f,  1.0f,  1.0f, 1.0f };
	vertexData_[1].position = { 1.0f,  1.0f, -1.0f, 1.0f };
	vertexData_[2].position = { 1.0f, -1.0f,  1.0f, 1.0f };
	vertexData_[3].position = { 1.0f, -1.0f, -1.0f, 1.0f };

	/* 左面。描画インデックスは[4,5,6][6,5,7] */
	vertexData_[4].position = { -1.0f,  1.0f, -1.0f, 1.0f };
	vertexData_[5].position = { -1.0f,  1.0f,  1.0f, 1.0f };
	vertexData_[6].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[7].position = { -1.0f, -1.0f,  1.0f, 1.0f };

	/* 前面。描画インデックスは[8,9,10][10,9,11] */
	vertexData_[8].position = { -1.0f,  1.0f,  1.0f, 1.0f };
	vertexData_[9].position = { 1.0f,  1.0f,  1.0f, 1.0f };
	vertexData_[10].position = { -1.0f, -1.0f,  1.0f, 1.0f };
	vertexData_[11].position = { 1.0f, -1.0f,  1.0f, 1.0f };

	/* 後面。描画インデックスは[12,13,14][14,13,15] */
	vertexData_[12].position = { 1.0f,  1.0f, -1.0f, 1.0f };
	vertexData_[13].position = { -1.0f,  1.0f, -1.0f, 1.0f };
	vertexData_[14].position = { 1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[15].position = { -1.0f, -1.0f, -1.0f, 1.0f };

	/* 上面。描画インデックスは[16,17,18][18,17,19] */
	vertexData_[16].position = { -1.0f,  1.0f, -1.0f, 1.0f };
	vertexData_[17].position = { 1.0f,  1.0f, -1.0f, 1.0f };
	vertexData_[18].position = { -1.0f,  1.0f,  1.0f, 1.0f };
	vertexData_[19].position = { 1.0f,  1.0f,  1.0f, 1.0f };

	/* 下面。描画インデックスは[20,21,22][22,21,23] */
	vertexData_[20].position = { -1.0f, -1.0f,  1.0f, 1.0f };
	vertexData_[21].position = { 1.0f, -1.0f,  1.0f, 1.0f };
	vertexData_[22].position = { -1.0f, -1.0f, -1.0f, 1.0f };
	vertexData_[23].position = { 1.0f, -1.0f, -1.0f, 1.0f };

	indexResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * uint32_t(36));
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * uint32_t(36);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	// 右面インデックス
	indexData_[0] = 0; indexData_[1] = 2; indexData_[2] = 1;
	indexData_[3] = 2; indexData_[4] = 3; indexData_[5] = 1;

	// 左面インデックス
	indexData_[6] = 4; indexData_[7] = 6; indexData_[8] = 5;
	indexData_[9] = 6; indexData_[10] = 7; indexData_[11] = 5;

	// 前面インデックス
	indexData_[12] = 8; indexData_[13] = 10; indexData_[14] = 9;
	indexData_[15] = 10; indexData_[16] = 11; indexData_[17] = 9;

	// 後面インデックス
	indexData_[18] = 12; indexData_[19] = 14; indexData_[20] = 13;
	indexData_[21] = 14; indexData_[22] = 15; indexData_[23] = 13;

	// 上面インデックス
	indexData_[24] = 16; indexData_[25] = 18; indexData_[26] = 17;
	indexData_[27] = 18; indexData_[28] = 19; indexData_[29] = 17;

	// 下面インデックス
	indexData_[30] = 20; indexData_[31] = 22; indexData_[32] = 21;
	indexData_[33] = 22; indexData_[34] = 23; indexData_[35] = 21;
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