#pragma once
#include <cstdint>
#include"Vector4.h"
#include"Color.h"
#include"Matrix4x4.h"
#include<d3d12.h>
#include<wrl.h>
#include"fstream"
#include"sstream"
#include<vector>


class Material
{
public:
	
public:// メンバ関数
	Material() = default;
	~Material() = default;
	
public:
	struct MaterialData {
		Color color;
		int enableLighting;
		float paddhing[3];
		Matrix4x4 uvTransform;
		float shininess;
	};
	struct OBJMaterialData
	{
		std::string textureFilePath;
		//std::vector<std::string> textureFilePath;
	};
	// ブレンドモード
	const std::vector<std::string> blendMode =
	{
		//!< ブレンドなし
		"kBlendModeNone",
		//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		"kBlendModeNormal",
		//!< 加算。Src * SrcA + Dest * 1
		"kBlendModeAdd",
		//!< 減算。Dest * 1 - Src * SrcA
		"kBlendModeSubtract",
		//!< 乗算。Src * 0 + Dest * Src
		"kBlendModeMultily",
		//!< スクリーン。Src * (1 - Dest) *Dest * 1
		"kBlendModeScreen",
	};
	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }

	void CreateMaterialResource();


	MaterialData* GetMaterialData()const { return materialData_; }
private:
	Microsoft::WRL::ComPtr < ID3D12Resource> materialResource_;
	MaterialData* materialData_ = nullptr;
};

