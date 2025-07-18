#include "GraphicsPipelineState.h"
#include"ConvertString.h"
/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
GraphicsPipelineState* GraphicsPipelineState::GetInstance() {
	static GraphicsPipelineState instance;
	return &instance;
}
void GraphicsPipelineState::CreateComputePipeline(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;
	
	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange = {};
	descriptorSRVRange.BaseShaderRegister = 0;
	descriptorSRVRange.NumDescriptors = 1;
	descriptorSRVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorSRVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange1 = {};
	descriptorSRVRange1.BaseShaderRegister = 1;
	descriptorSRVRange1.NumDescriptors = 1;
	descriptorSRVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorSRVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorSRVRange2 = {};
	descriptorSRVRange2.BaseShaderRegister = 2;
	descriptorSRVRange2.NumDescriptors = 1;
	descriptorSRVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorSRVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorSRVRange2;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature_));
	assert(SUCCEEDED(hr));

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob = CompilerShader(L"Resources/Shader/Skinning.CS.hlsl",
		L"cs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(computeShaderBlob != nullptr);

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.pRootSignature = rootSignature_.Get();  // RootSignature
	computePipelineStateDesc.CS = { 
	.pShaderBytecode=computeShaderBlob->GetBufferPointer(),
	.BytecodeLength=computeShaderBlob->GetBufferSize() };                       // VertexShader
	computePipelineStateDesc.pRootSignature = computeRootSignature_.Get();
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&computePipelineState_));
	assert(SUCCEEDED(hr));
}
void GraphicsPipelineState::CreateComputePipelineParticle(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[3] = {};



	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数


	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&computeRootSignatureParticle_));
	assert(SUCCEEDED(hr));

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob = CompilerShader(L"Resources/Shader/Particle.CS.hlsl",
		L"cs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(computeShaderBlob != nullptr);

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };                       // VertexShader
	computePipelineStateDesc.pRootSignature = computeRootSignatureParticle_.Get();
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&computePipelineStateParticle_));
	assert(SUCCEEDED(hr));
}
void GraphicsPipelineState::CreateGraphicsPipelineGPUParticle(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;
	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[3] = {};

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// PerView
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ



	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);



	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureGPUParticle_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "COLOR";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/GPUParticle.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/GPUParticle.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureGPUParticle_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader                     
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGPUParticle_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGPUParticle_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGPUParticle_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGPUParticle_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGPUParticle_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGPUParticle_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));
}
void GraphicsPipelineState::CreateComputePipelineEmit(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[5] = {};



	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[2].Descriptor.ShaderRegister = 1;// レジスタ番号1とバインド

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[4].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数


	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&computeRootSignatureEmit_));
	assert(SUCCEEDED(hr));

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob = CompilerShader(L"Resources/Shader/EmitParticle.CS.hlsl",
		L"cs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(computeShaderBlob != nullptr);

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };                       // VertexShader
	computePipelineStateDesc.pRootSignature = computeRootSignatureEmit_.Get();
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&computePipelineStateEmit_));
	assert(SUCCEEDED(hr));
}
void GraphicsPipelineState::CreateComputePipelineUpdate(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[4] = {};



	D3D12_DESCRIPTOR_RANGE descriptorUAVRange = {};
	descriptorUAVRange.BaseShaderRegister = 0;
	descriptorUAVRange.NumDescriptors = 1;
	descriptorUAVRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &descriptorUAVRange;// Tableの中身の配列を指定
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange1 = {};
	descriptorUAVRange1.BaseShaderRegister = 1;
	descriptorUAVRange1.NumDescriptors = 1;
	descriptorUAVRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorUAVRange1;// Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE descriptorUAVRange2 = {};
	descriptorUAVRange2.BaseShaderRegister = 2;
	descriptorUAVRange2.NumDescriptors = 1;
	descriptorUAVRange2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorUAVRange2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorUAVRange2;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数


	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ


	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&computeRootSignatureUpdate_));
	assert(SUCCEEDED(hr));

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> computeShaderBlob = CompilerShader(L"Resources/Shader/UpdateParticle.CS.hlsl",
		L"cs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(computeShaderBlob != nullptr);

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
	.pShaderBytecode = computeShaderBlob->GetBufferPointer(),
	.BytecodeLength = computeShaderBlob->GetBufferSize() };                       // VertexShader
	computePipelineStateDesc.pRootSignature = computeRootSignatureUpdate_.Get();
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc,
		IID_PPV_ARGS(&computePipelineStateUpdate_));
	assert(SUCCEEDED(hr));
}
/// <summary>
/// dxcCompilerを初期化
/// </summary>
void GraphicsPipelineState::InitializeDxcCompiler() {
	HRESULT hr;
	// dxcCompilerを初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するための設定を作っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}
/// <summary>
/// グラフィックスパイプラインの作成
/// </summary>
void GraphicsPipelineState::CreateGraphicsPipeline(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	//// RootSignature作成
	//// obj3d用
	//D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	//descriptionRooTSignature.Flags =
	//	D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	//D3D12_ROOT_PARAMETER rootParameter[4] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド
	//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//descriptorRange[0].BaseShaderRegister = 0;// ０から始まる
	//descriptorRange[0].NumDescriptors = 1;// 数は1つ
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算
	//rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	//rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド
	//
	//
	//rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	//rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;// Tableの中身の配列
	//rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);// Tableで利用する数
	//rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う
	//descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	//descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[8] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS1 = {};
	srvDescriptorRangePS1.BaseShaderRegister = 1;
	srvDescriptorRangePS1.NumDescriptors = 1;
	srvDescriptorRangePS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// ライト
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う

	// WVPデータ
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド

	// ポイントライト
	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[5].Descriptor.ShaderRegister = 2;// レジスタ番号2とバインド

	// スポットライト
	rootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[6].Descriptor.ShaderRegister = 3;// レジスタ番号3とバインド

	// 環境マップ
	rootParameter[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[7].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS1;// Tableの中身の配列
	rootParameter[7].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITIONT";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/obj.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	/*Microsoft::WRL::ComPtr < IDxcBlob> GeometryShaderBlob = CompilerShader(L"Resources/Shader/BasicGeometryShader.hlsl",
		L"gs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(GeometryShaderBlob != nullptr);*/

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/obj.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

/// <summary>
/// パーティクル用のグラフィックスパイプラインの作成
/// </summary>
void GraphicsPipelineState::CreateParticleGraphicsPipeline(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[6] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// ライト
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う

	// WVPデータ
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用の追加SRVテーブル
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS2 = {};
	srvDescriptorRangeVS2.BaseShaderRegister = 1; // シェーダーレジスタを変更
	srvDescriptorRangeVS2.NumDescriptors = 1;
	srvDescriptorRangeVS2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// カラーデータ
	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[5].DescriptorTable.pDescriptorRanges =&srvDescriptorRangeVS2;// Tableの中身の配列を指定
	rootParameter[5].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);



	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureParticle_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "COLOR";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/Particle.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Particle.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureParticle_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader                     
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

/// <summary>
/// グラフィックスパイプラインの作成
/// </summary>
void GraphicsPipelineState::CreateGraphicsPipelineGSO(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	

	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	

	// WVPデータ
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド

	

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureGSO_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/Primitive.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> GeometryShaderBlob = CompilerShader(L"Resources/Shader/BasicGeometryShader.hlsl",
		L"gs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(GeometryShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Primitive.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		//D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateGSO_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

void GraphicsPipelineState::CreateGraphicsPipelineSprite(ID3D12Device* device) {
	InitializeDxcCompiler();
	HRESULT hr;
	
	// RootSignature作成
	
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 1;// レジスタ番号1とバインド

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	

	// WVPデータ
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド

	

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureSprite_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/Sprite.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Sprite.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureSprite_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSprite_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSprite_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSprite_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSprite_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSprite_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSprite_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));


}

void GraphicsPipelineState::CreateGraphicsPipelineDepthFilter(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;
	

	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[3] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド


	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS1 = {};
	srvDescriptorRangePS1.BaseShaderRegister = 1;
	srvDescriptorRangePS1.NumDescriptors = 1;
	srvDescriptorRangePS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS1;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	//staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[1].ShaderRegister = 1;// レジスタ番号を使う
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureDepthFilter_));
	assert(SUCCEEDED(hr));

	// InputLayout
	

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/FullScreen.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/DepthBasedOutline.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureDepthFilter_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDepthFilter_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDepthFilter_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDepthFilter_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDepthFilter_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDepthFilter_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDepthFilter_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

}

void GraphicsPipelineState::CreateGraphicsPipelineCopy(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;


	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[1] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド


	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureCopy_));
	assert(SUCCEEDED(hr));

	// InputLayout


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/FullScreen.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/FullScreen.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureCopy_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateCopy_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateCopy_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateCopy_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateCopy_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateCopy_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateCopy_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

}

void GraphicsPipelineState::CreateGraphicsPipelineDissolve(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;


	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[2] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド


	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS1 = {};
	srvDescriptorRangePS1.BaseShaderRegister = 1;
	srvDescriptorRangePS1.NumDescriptors = 1;
	srvDescriptorRangePS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// テクスチャ
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS1;// Tableの中身の配列
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureDissolve_));
	assert(SUCCEEDED(hr));

	// InputLayout


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/FullScreen.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Dissolve.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureDissolve_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDissolve_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDissolve_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDissolve_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDissolve_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDissolve_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateDissolve_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));

}

void GraphicsPipelineState::CreateGraphicsPipelineRandom(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;


	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[2] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド


	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureRandom_));
	assert(SUCCEEDED(hr));

	// InputLayout


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/FullScreen.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Random.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureRandom_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateRandom_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateRandom_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateRandom_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateRandom_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateRandom_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateRandom_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));
}

void GraphicsPipelineState::CreateGraphicsPipelineHSV(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;


	// RootSignature作成
	// copy用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[2] = {};
	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	//rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド


	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// テクスチャ
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[1].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureHSV_));
	assert(SUCCEEDED(hr));

	// InputLayout


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/FullScreen.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/ToHSV.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureHSV_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateHSV_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateHSV_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateHSV_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateHSV_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateHSV_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateHSV_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));
}

void GraphicsPipelineState::CreateGraphicsPipelineSkinning(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;

	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[9] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS2 = {};
	srvDescriptorRangeVS2.BaseShaderRegister = 1;
	srvDescriptorRangeVS2.NumDescriptors = 1;
	srvDescriptorRangeVS2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS1 = {};
	srvDescriptorRangePS1.BaseShaderRegister = 1;
	srvDescriptorRangePS1.NumDescriptors = 1;
	srvDescriptorRangePS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// World
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// テクスチャ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// ライト
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[3].Descriptor.ShaderRegister = 1;// レジスタ番号1を使う

	// WVPデータ
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[4].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド

	// ポイントライト
	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[5].Descriptor.ShaderRegister = 2;// レジスタ番号2とバインド

	// スポットライト
	rootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[6].Descriptor.ShaderRegister = 3;// レジスタ番号3とバインド

	rootParameter[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[7].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS2;// Tableの中身の配列を指定
	rootParameter[7].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// 環境マップ
	rootParameter[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[8].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS1;// Tableの中身の配列
	rootParameter[8].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureSkinning_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};
	inputElementDescs[0].SemanticName = "POSITIONT";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[3].SemanticName = "WEIGHT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;// float32_t4
	inputElementDescs[3].InputSlot = 1;// 1番目のslotのVBVのことだと伝える
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[4].SemanticName = "INDEX";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;// int32_t4
	inputElementDescs[4].InputSlot = 1;// 1番目のslotのVBVのことだと伝える
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/SkinningObject3d.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	/*Microsoft::WRL::ComPtr < IDxcBlob> GeometryShaderBlob = CompilerShader(L"Resources/Shader/BasicGeometryShader.hlsl",
		L"gs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(GeometryShaderBlob != nullptr);*/

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/obj.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureSkinning_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	/*graphicsPipelineStateDesc.GS = { GeometryShaderBlob->GetBufferPointer(),
	GeometryShaderBlob->GetBufferSize() };*/
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkinning_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkinning_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkinning_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkinning_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkinning_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkinning_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));
}

void GraphicsPipelineState::CreateGraphicsPipelineSkybox(ID3D12Device* device)
{
	InitializeDxcCompiler();
	HRESULT hr;

	// RootSignature作成
	// obj3d用
	D3D12_ROOT_SIGNATURE_DESC descriptionRooTSignature{};
	descriptionRooTSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[0].Descriptor.ShaderRegister = 0;// レジスタ番号０とバインド

	// Vertex Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangeVS1 = {};
	srvDescriptorRangeVS1.BaseShaderRegister = 0;
	srvDescriptorRangeVS1.NumDescriptors = 1;
	srvDescriptorRangeVS1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangeVS1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Pixel Shader用のSRV
	D3D12_DESCRIPTOR_RANGE srvDescriptorRangePS = {};
	srvDescriptorRangePS.BaseShaderRegister = 0;
	srvDescriptorRangePS.NumDescriptors = 1;
	srvDescriptorRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescriptorRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	// テクスチャ
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderを使う
	rootParameter[1].DescriptorTable.pDescriptorRanges = &srvDescriptorRangePS;// Tableの中身の配列
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	// WVPデータ
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[2].Descriptor.ShaderRegister = 0;// レジスタ番号2とバインド


	// World
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderを使う
	rootParameter[3].DescriptorTable.pDescriptorRanges = &srvDescriptorRangeVS1;// Tableの中身の配列を指定
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;// Tableで利用する数

	descriptionRooTSignature.pParameters = rootParameter;// ルートパラメータ配列へのポインタ
	descriptionRooTSignature.NumParameters = _countof(rootParameter);// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;// レジスタ番号を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRooTSignature.pStaticSamplers = staticSamplers;
	descriptionRooTSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRooTSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(SUCCEEDED(hr));
	}

	// バイナリをもとに生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignatureSkybox_));
	assert(SUCCEEDED(hr));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);



	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しな
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = CompilerShader(L"Resources/Shader/Skybox.VS.hlsl",
		L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = CompilerShader(L"Resources/Shader/Skybox.PS.hlsl",
		L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みをします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureSkybox_.Get();  // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;   // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };                       // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };                        // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
// どのように画面に色を打ち込むかの設定（気にしなくてもいい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkybox_[kBlendModeNone]));
	assert(SUCCEEDED(hr));

	// ここからブレンドPSOの各設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkybox_[kBlendModeNormal]));
	assert(SUCCEEDED(hr));

	//!< 加算。Src * SrcA + Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkybox_[kBlendModeAdd]));
	assert(SUCCEEDED(hr));

	//!< 減算。Dest * 1 - Src * SrcA
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkybox_[kBlendModeSubtract]));
	assert(SUCCEEDED(hr));

	//!< 乗算。Src * 0 + Dest * Src
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkybox_[kBlendModeMultily]));
	assert(SUCCEEDED(hr));

	//!< スクリーン。Src * (1 - Dest) *Dest * 1
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;          // BlendState
	hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateSkybox_[kBlendModeScreen]));
	assert(SUCCEEDED(hr));
}

IDxcBlob* GraphicsPipelineState::CompilerShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	HRESULT hr = {};
	Log(ConvertString(std::format(L"Begin CompilerShader,path:{},profile:{}\n", filePath, profile)).c_str());
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> pSource = nullptr;
	hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &pSource);
	assert(SUCCEEDED(hr));
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = pSource->GetBufferPointer();
	sourceBuffer.Size = pSource->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_UTF8;
	LPCWSTR arguments[] = {
		filePath.c_str(),       //コンパイル対象のhlslファイル名
		L"-E",L"main",          // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,          // ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",// デバッグ用の情報を埋め込む
		L"-Od",                 // 最適化を外しておく
		L"-Zpr",                // メモリレイアウトは行優先
	};
	Microsoft::WRL::ComPtr<IDxcResult> pResult = nullptr;
	hr = dxcCompiler->Compile(
		&sourceBuffer,            // 読み込んだファイル
		arguments,                // コンパイルオプション
		_countof(arguments),    // コンパイル結果
		includeHandler,// includeが含まれた諸々
		IID_PPV_ARGS(&pResult)    // コンパイル結果
	);
	assert(SUCCEEDED(hr));
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> pErrors = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlobUtf16> pErrorsUtf16;
	hr = pResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), &pErrorsUtf16);
	if (pErrors != nullptr && pErrors->GetStringLength() != 0)
	{
		Log(pErrors->GetStringPointer());
		Log("DXC Compile Error");
	}
	IDxcBlob* pShader = nullptr;
	hr = pResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pErrorsUtf16);
	assert(SUCCEEDED(hr));
	Log(ConvertString(std::format(L"End CompilerShader,path:{},profile:{}\n", filePath, profile)).c_str());
	return pShader;
}