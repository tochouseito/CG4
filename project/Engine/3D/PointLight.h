#pragma once
#include"Vector4.h"
#include"Vector3.h"
#include"Color.h"
#include"d3d12.h"
#include"wrl.h"
class PointLight
{



public:
	PointLight() = default;
	~PointLight() = default;
	struct PointLightData
	{
		Color color;//!<ライトの色
		Vector3 position;//!<ライトの位置
		float intensity;//!<輝度
		float radius;//!<ライトの届く最大距離
		float decay;//!<減衰率
		float padding[2];
	};
	void CreatePointLightResource();
	ID3D12Resource* GetLightResource() { return PointLightResource_.Get(); }
	PointLightData* GetPointLightData() { return PointLightData_; }
private:
	Microsoft::WRL::ComPtr< ID3D12Resource> PointLightResource_;
	PointLightData* PointLightData_ = { nullptr };

};

