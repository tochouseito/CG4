#pragma 
#include"DirectXCommon.h"
class GraphicsPipelineState
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static GraphicsPipelineState* GetInstance();

	enum BlendMode {
		//!< ブレンドなし
		kBlendModeNone,
		//!< 通常のaブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		kBlendModeNormal,
		//!< 加算。Src * SrcA + Dest * 1
		kBlendModeAdd,
		//!< 減算。Dest * 1 - Src * SrcA
		kBlendModeSubtract,
		//!< 乗算。Src * 0 + Dest * Src
		kBlendModeMultily,
		//!< スクリーン。Src * (1 - Dest) *Dest * 1
		kBlendModeScreen,
		//!< 利用してはいけない
		kCountOfBlendMode,
	};
private:// メンバ関数
	GraphicsPipelineState() = default;
	~GraphicsPipelineState() = default;
	GraphicsPipelineState(const GraphicsPipelineState&) = delete;
	const GraphicsPipelineState& operator=(const GraphicsPipelineState&) = delete;
public:

	static ID3D12PipelineState* GetPipelineState(uint32_t BlendMode) { return GetInstance()->graphicsPipelineState_[BlendMode].Get(); }
	static ID3D12PipelineState* GetParticlePipelineState(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateParticle_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateGSO(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateGSO_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateSprite(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateSprite_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateCopy(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateCopy_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateDepthFilter(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateDepthFilter_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateDissolve(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateDissolve_[BlendMode].Get(); }
	static ID3D12PipelineState* GetPipelineStateHSV(uint32_t BlendMode) { return GetInstance()->graphicsPipelineStateHSV_[BlendMode].Get(); }

	static ID3D12RootSignature* GetRootSignature() { return GetInstance()->rootSignature_.Get(); }
	static ID3D12RootSignature* GetRootSignatureParticle() { return GetInstance()->rootSignatureParticle_.Get(); }
	static ID3D12RootSignature* GetRootSignatureGSO() { return GetInstance()->rootSignatureGSO_.Get(); }
	static ID3D12RootSignature* GetRootSignatureSprite() { return GetInstance()->rootSignatureSprite_.Get(); }
	static ID3D12RootSignature* GetRootSignatureCopy() { return GetInstance()->rootSignatureCopy_.Get(); }
	static ID3D12RootSignature* GetRootSignatureDepthFilter() { return GetInstance()->rootSignatureDepthFilter_.Get(); }
	static ID3D12RootSignature* GetRootSignatureDissolve() { return GetInstance()->rootSignatureDissolve_.Get(); }
	static ID3D12RootSignature* GetRootSignatureRandom() {return GetInstance()->rootSignatureRandom_.Get(); }
	static ID3D12RootSignature* GetRootSignatureHSV() { return GetInstance()->rootSignatureHSV_.Get(); }

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipeline(ID3D12Device* device);

	/// <summary>
	/// パーティクル用のグラフィックスパイプラインの作成
	/// </summary>
	void CreateParticleGraphicsPipeline(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineGSO(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineSprite(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineDepthFilter(ID3D12Device* device);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="device"></param>
	void CreateGraphicsPipelineCopy(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineDissolve(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineRandom(ID3D12Device* device);

	/// <summary>
	/// グラフィックスパイプラインの作成
	/// </summary>
	void CreateGraphicsPipelineHSV(ID3D12Device* device);

	/// <summary>
	/// dxcCompilerを初期化
	/// </summary>
	void InitializeDxcCompiler();

	/// <summary>
	/// shaderのコンパイル
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <param name="dxcUtils"></param>
	/// <param name="dxcCompiler"></param>
	/// <param name="includeHandler"></param>
	/// <returns></returns>
	IDxcBlob* CompilerShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
private:
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateParticle_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateGSO_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateSprite_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateCopy_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateDepthFilter_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateDissolve_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateRandom_[kCountOfBlendMode];
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateHSV_[kCountOfBlendMode];
	//ID3D12PipelineState* graphicsPipelineState_;
	Microsoft::WRL::ComPtr < IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature>rootSignatureParticle_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureGSO_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureSprite_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureCopy_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureDepthFilter_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureDissolve_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureRandom_;
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureHSV_;
};

