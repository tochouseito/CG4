#pragma once
#include"Vector4.h"
#include"Vector3.h"
#include"Color.h"
#include"d3d12.h"
#include"wrl.h"
class SpotLight
{
public:
	SpotLight() = default;
	~SpotLight() = default;
	struct SpotLightData
	{
		Color color;//!<ライトの色
		Vector3 position; //!<ライトの位置
		float intensity; //!<輝度
		Vector3 direction;//!<スポットライトの方向
		float distance;//!<ライトの届く最大距離
		float decay;//!<減衰率
		float cosAngle;//!<スポットライトの余弦
		float cosFalloffStart;//Falloff開始の角度
		float padding[1];
	};
	void CreateSpotLightResource();
	ID3D12Resource* GetLightResource() { return SpotLightResource_.Get(); }
	SpotLightData* GetSpotLightData() { return SpotLightData_; }
private:
	Microsoft::WRL::ComPtr< ID3D12Resource> SpotLightResource_;
	SpotLightData* SpotLightData_ = { nullptr };

};