#include"Obj3d.hlsli"
cbuffer Material : register(b0)
{
    float4 colorMaterial : SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};
cbuffer DirectionalLight : register(b1)
{
    float4 colorLight; //!<ライトの色
    float3 direction; //!<ライトの向き
    float intensity; //!<輝度
};
struct PointLight
{
    float4 color;//!<ライトの色
    float3 position;//!<ライトの位置
    float intensity;//!<輝度
    float radius;//!<ライトの届く最大距離
    float decay;//!<減衰率
};
struct SpotLight
{
    float4 color;//!<ライトの色
    float3 position; //!<ライトの位置
    float intensity; //!<輝度
    float3 direction;//!<スポットライトの方向
    float distance;//!<ライトの届く最大距離
    float decay;//!<減衰率
    float cosAngle;//!<スポットライトの余弦
    float cosFalloffStart;//Falloff開始の角度
};
ConstantBuffer<PointLight> gPointLight : register(b2);
ConstantBuffer<SpotLight> gSpotLight : register(b3);

struct PixelShaderOutput
{
    float4 colorPS : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
// 環境マップによるライティング
TextureCube<float4> gEnvironmentTexture : register(t1);
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // カメラへの方向を算出
    float3 toEye = normalize(input.cameraPosition - input.worldPosition);
    
    // 入射光の反射ベクトルを求める
    float3 reflectLight = reflect(direction, normalize(input.normal));
    // 鏡面反射の強度を求める
    float3 halfVector = normalize(-direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector); // 鏡面反射強度
    float specularPow = pow(saturate(NDotH), shininess);
    
    // PointLight
    // 物体の特定の点に対する入射光を計算する
    float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
    // 入射光の反射ベクトルを求める
    float3 reflectPointLight = reflect(pointLightDirection, normalize(input.normal));
    // 鏡面反射の強度を求める
    float3 halfVectorPointLight = normalize(-pointLightDirection + toEye);
    float NDotHPointLight = dot(normalize(input.normal), halfVectorPointLight); // 鏡面反射強度
    float specularPowPointLight = pow(saturate(NDotHPointLight), shininess);
    // 減衰係数の計算
    float distance = length(gPointLight.position - input.worldPosition);// ポイントライトへの距離
    float factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay); // 逆二乗測による係数,指数によるコントロール
    
    // SpotLight
    // 物体の特定の点に対する入射光を計算する
    float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
    // 入射光の反射ベクトルを求める
    float3 reflectSpotLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
    // 鏡面反射の強度を求める
    float3 halfVectorSpotLight = normalize(-spotLightDirectionOnSurface + toEye);
    float NDotHSpotLight = dot(normalize(input.normal), halfVectorSpotLight); // 鏡面反射強度
    float specularPowSpotLight = pow(saturate(NDotHSpotLight), shininess);
    // 減衰係数の計算
    float distanceSpotLight = length(gSpotLight.position - input.worldPosition); // ポイントライトへの距離
    float factorSpotLight = pow(saturate(-distanceSpotLight / gSpotLight.distance + 1.0), gSpotLight.decay); // 逆二乗測による係数,指数によるコントロール
    float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
    float falloffFactor;
    if (gSpotLight.cosAngle == gSpotLight.cosFalloffStart)
    {
        falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (1.0f - gSpotLight.cosAngle));
    }
    else
    {
        falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
    }
    
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    // textureのa値が0のときにPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    if (textureColor.a == 0.0)
    {
        discard;
    }
    if (enableLighting != 0)
    {
        // DirectionalLight
        // half lambert
            float NdotL = dot(normalize(input.normal), -direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        //output.colorPS = color * textureColor * colorLight * cos * intensity;
        // 拡散反射
        float3 diffuseDirectionalLight =
        colorMaterial.rgb * textureColor.rgb * colorLight.rgb * cos * intensity;
        // 鏡面反射
        float3 specularDirectionalLight =
        colorLight.rgb * intensity * specularPow * float3(1.0f, 1.0f, 1.0f);
        
        
        // PointLight
        
        // half lambert
        float NdotLPointLight = dot(normalize(input.normal), -pointLightDirection);
        float cosPointLight = pow(NdotLPointLight * 0.5f + 0.5f, 2.0f);
        // 拡散反射
        float3 diffusePointLight =
        colorMaterial.rgb * textureColor.rgb * gPointLight.color.rgb * cosPointLight * gPointLight.intensity * factor;
        // 鏡面反射
        float3 specularPointLight =
        gPointLight.color.rgb * gPointLight.intensity * factor * specularPowPointLight * float3(1.0f, 1.0f, 1.0f);
        
        // SpotLight
        // half lambert
        float NdotLSpotLight = dot(normalize(input.normal), -spotLightDirectionOnSurface);
        float cosSpotLight = pow(NdotLSpotLight * 0.5f + 0.5f, 2.0f);
        // 拡散反射
        float3 diffuseSpotLight =
        colorMaterial.rgb * textureColor.rgb * gSpotLight.color.rgb * cosSpotLight * gSpotLight.intensity * factorSpotLight*falloffFactor;
        // 鏡面反射
        float3 specularSpotLight =
        gSpotLight.color.rgb * gSpotLight.intensity * factorSpotLight * falloffFactor * specularPowSpotLight * float3(1.0f, 1.0f, 1.0f);
        
        // 映り込み
        float3 cameraToPosition = normalize(input.worldPosition - input.cameraPosition);
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
        
        // 拡散反射+鏡面反射
        output.colorPS.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight+diffuseSpotLight + specularSpotLight;
        output.colorPS.a = colorMaterial.a * textureColor.a;
        // 映り込み追加
        output.colorPS.rgb += environmentColor.rgb;
        if (output.colorPS.a == 0.0)
        {
            discard;
        }
    }
    else
    {
        output.colorPS = colorMaterial * textureColor;
    }
    return output;
}
