#pragma once
#include"Vector4.h"
#include"Vector3.h"
#include"Color.h"
#include"d3d12.h"
#include"wrl.h"
class DirectionalLight
{
public:
	DirectionalLight() = default;
	~DirectionalLight() = default;
	struct DirectionalLightData
	{
		Color color;//!<ライトの色
		Vector3 direction;//!<ライトの向き
		float intensity;//!<輝度
	};
	void CreateDirectionalLightResource();
	ID3D12Resource* GetLightResource() { return directionalLightResource_.Get(); }
	DirectionalLightData* GetDirectionalLightData() { return directionalLightData_; }
private:
	Microsoft::WRL::ComPtr< ID3D12Resource> directionalLightResource_;
	DirectionalLightData* directionalLightData_ = { nullptr };
};

