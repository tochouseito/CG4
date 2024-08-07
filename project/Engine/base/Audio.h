#pragma once
#include<xaudio2.h>
#include<fstream>
#include <wrl.h>

#pragma comment(lib,"xaudio2.lib")
class Audio
{
public:/*ファイル構造体*/
	/*チャンクヘッダ*/
	struct ChunkHeader
	{
		char id[4];/*チャンク毎のID*/
		int32_t size;/*チャンクのサイズ*/
	};
	/*RIFFヘッダチャンク*/
	struct RiffHeader
	{
		ChunkHeader chunk;/*”RIFF"*/
		char type[4];/*"WAVE"*/
	};
	/*FMTチャンク*/
	struct FormatChunk
	{
		ChunkHeader chunk;/*"fmt"*/
		WAVEFORMATEX fmt;/*波形フォーマット*/
	};
	/*音声データ*/
	struct SoundData
	{
		/*波形フォーマット*/
		WAVEFORMATEX wfex;
		/*バッファの先頭アドレス*/
		BYTE* pBuffer;
		/*バッファのサイズ*/
		unsigned int bufferSize;

	};
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static Audio* GetInstance();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	SoundData SoundLordWave(const char* filename);

	/*音声データ解放*/
	void SoundUnlord(SoundData* soundData);

	/*音声再生*/
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	IXAudio2* GetXAudio2() { return xAudio2.Get(); }
	
private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
};

