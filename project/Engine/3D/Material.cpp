#include "Material.h"
#include"Mymath.h"
#include"DirectXCommon.h"

void Material::CreateMaterialResource() {
	// マテリアル用のリソースを作る。今回はcolor1tu分のサイズを用意する
	materialResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(MaterialData));
	// マテリアルデータを書き込む
	//Vector4* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでいる
	materialData_->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 50.0f;
}
