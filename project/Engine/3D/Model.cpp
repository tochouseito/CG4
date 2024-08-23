#include "Model.h"
#include"GraphicsPipelineState.h"
#include"DirectXCommon.h"
#include"TextureManager.h"
#include"imgui.h"
#include <iostream>
#include"fstream"
#include"sstream"
#include<random>
#include<numbers>
#include"Mymath.h"
#include"SrvManager.h"
#include<assert.h>
Model::Model() {
	material_ = new Material();
	directionalLight_ = new DirectionalLight();
	pointLight_ = new PointLight();
	spotLight_ = new SpotLight();
	mesh_ = new Mesh();
}
Model::~Model() {
	delete mesh_;
	delete material_;
	delete directionalLight_;
	delete pointLight_;
	delete spotLight_;
	delete skinCluster_;
}

/*平面オブジェクト*/
Model* Model::CreatePlane() {
	Model* model = new Model();
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	model->mesh_->CreateObjectVertex(Mesh::Object::kPlane);
	model->material_->CreateMaterialResource();
	model->directionalLight_->CreateDirectionalLightResource();
	model->pointLight_->CreatePointLightResource();
	model->spotLight_->CreateSpotLightResource();
	return model;
}
/*球オブジェクト*/
Model* Model::CreateSphere(uint32_t Subdivision) {
	Model* model = new Model();
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	model->mesh_->CreateObjectVertex(Mesh::Object::kSphere);
	model->material_->CreateMaterialResource();
	model->directionalLight_->CreateDirectionalLightResource();
	model->pointLight_->CreatePointLightResource();
	model->spotLight_->CreateSpotLightResource();
	return model;
}

Vector3 Model::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) /( keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion Model::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Model::Skeleton* Model::CreateSkeleton(const Node& rootNode)
{
	Skeleton* skeleton=new Skeleton();
	skeleton->root = CreateJoint(rootNode, {}, skeleton->joints);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton->joints) {
		skeleton->jointMap.emplace(joint.name, joint.index);
	}
	return skeleton;
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = static_cast<int32_t>(joints.size());// 現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);// SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自身のIndexを返す
	return joint.index;
	/*
	本来はanimationするNodeのみを対象にしたほうがいいが今は全Nodeを対象にしている
	*/
}

void Model::SkeletonUpdata(Skeleton* skeleton)
{
	// すべてのJointを更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeleton->joints) {
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton->joints[*joint.parent].skeletonSpaceMatrix;
		} else {// 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Model::ApplyAnimation(Skeleton* skeleton, const Animation* animation, float animationTime)
{
	for (Joint& joint : skeleton->joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文
		if (auto it = animation->nodeAnimations.find(joint.name); it != animation->nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

Model::SkinCluster* Model::CreateSkinCluster(const Microsoft::WRL::ComPtr<ID3D12Device>& device,  Skeleton* skeleton, 
	ObjectData& objectData)
{
	SkinCluster* skinCluster=new SkinCluster();
	/*palette用のResourceを確保*/
	skinCluster->paletteResource = DirectXCommon::GetInstance()->CreateBufferResource(device.Get(), sizeof(WellForGPU) * skeleton->joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster->paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster->mappedPalette = { mappedPalette,skeleton->joints.size() };// spanを使ってアクセスする
	/*palette用のsrvを作成*/
	skinCluster->srvIndex = SrvManager::GetInstance()->Allocate();
	skinCluster->paletteSrvHandle.first = SrvManager::GetInstance()->GetCPUDescriptorHandle(skinCluster->srvIndex);
	skinCluster->paletteSrvHandle.second = SrvManager::GetInstance()->GetGPUDescriptorHandle(skinCluster->srvIndex);
	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(skinCluster->srvIndex, skinCluster->paletteResource.Get(), static_cast<UINT>(skeleton->joints.size()), sizeof(WellForGPU));
	/*influencey用のResourceを確保。頂点ごとにInfluence情報を追加できるようにする*/
	skinCluster->influenceResource = DirectXCommon::GetInstance()->CreateBufferResource(device.Get(), sizeof(VertexInfluence) * objectData.vertices.size());
	VertexInfluence* mappedInflence = nullptr;
	skinCluster->influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInflence));
	std::memset(mappedInflence, 0, sizeof(VertexInfluence) * objectData.vertices.size());// 0埋め。weightを0にしておく
	skinCluster->mappedInfluence = { mappedInflence,objectData.vertices.size() };
	/*Influence用のVBVを作成*/
	skinCluster->influenceBufferView.BufferLocation = skinCluster->influenceResource->GetGPUVirtualAddress();
	skinCluster->influenceBufferView.SizeInBytes = static_cast<UINT>(sizeof(VertexInfluence) * objectData.vertices.size());
	skinCluster->influenceBufferView.StrideInBytes = sizeof(VertexInfluence);
	/*InverseBindPoseMatrixの保存領域を作成*/
	skinCluster->inverseBindPoseMatrices.resize(skeleton->joints.size());
	std::generate(skinCluster->inverseBindPoseMatrices.begin(), skinCluster->inverseBindPoseMatrices.end(), []() { return MakeIdentity4x4(); });
	/*ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める*/
	for (const auto& jointWeight : objectData.skinClusterData) {// ModelのSkinClusterの情報を解析
		auto it = skeleton->jointMap.find(jointWeight.first);// jointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		if (it == skeleton->jointMap.end()) {// そんな名前のjointは存在しない、なので次に回す
			continue;
		}
		/*(*it).secondにはJointのIndexが入っているので、該当のindexのInverseBindPoseMatrixを代入*/
		skinCluster->inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster->mappedInfluence[vertexWeight.vertexIndex];// 該当のvertexIndexのinfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {// 空いてるところに入れる
				if (currentInfluence.weights[index] == 0.0f) {// weight==0が空いてる状態なので、その場所にweightとjointのIndexを代入
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
	return skinCluster;
}

void Model::SkinClusterUpdata(SkinCluster* skinCluster, Skeleton* skeleton)
{
	for (size_t jointIndex = 0; jointIndex < skeleton->joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster->inverseBindPoseMatrices.size());
		skinCluster->mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster->inverseBindPoseMatrices[jointIndex] * skeleton->joints[jointIndex].skeletonSpaceMatrix;
		skinCluster->mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Transpose(Inverse(skinCluster->mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

/// <summary>
/// 描画
/// </summary>
void Model::Draw(WorldTransform& worldTransform,ViewProjection& viewProjection,
	std::string textureHandle) {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	if (modelData_->bone) {/*ボーンあり*/
		commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignatureSkinning());
		commandList->SetPipelineState(GraphicsPipelineState::GetPipelineStateSkinning(current_blend));// PSOを設定
	} else/*ボーンなし*/
	{
		commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignature());
		commandList->SetPipelineState(GraphicsPipelineState::GetPipelineState(current_blend));// PSOを設定
	}
	if (modelData_) {
		for (std::string name : modelData_->names) {
			if (modelData_->bone) {
				D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
					mesh_->GetVBV(name),
					skinCluster_->influenceBufferView,
				};
				commandList->IASetVertexBuffers(0, 2, vbvs);
			} else
			{
				commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView(name));// VBVを設定
			}
			D3D12_INDEX_BUFFER_VIEW* v = mesh_->GetIndexBufferView(name);
			commandList->IASetIndexBuffer(v);
			if (useMonsterBall) {
				commandList->IASetIndexBuffer(mesh_->GetIndexBufferView());// IBVを設定
			}
			// マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());

			// wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(4, viewProjection.GetWvpResource()->GetGPUVirtualAddress());
			// wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(3, directionalLight_->GetLightResource()->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(5, pointLight_->GetLightResource()->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(6, spotLight_->GetLightResource()->GetGPUVirtualAddress());
			if (modelData_->object[name].useTexture) {
				commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(modelData_->object[name].material.textureFilePath));
			} else {
				commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle));
			}
			commandList->SetGraphicsRootDescriptorTable(1, worldTransform.GetSrvHandleGPU());
			if (modelData_->bone) {
				commandList->SetGraphicsRootDescriptorTable(7, skinCluster_->paletteSrvHandle.second);
			}
			// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
			//commandList->DrawInstanced(static_cast<UINT>(mesh_->GetDataVertices(name)), 1, 0, 0);
			commandList->DrawIndexedInstanced(static_cast<UINT>(GetIndices(name)), 1, 0, 0, 0);
		}
	} else {
		commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView());// VBVを設定
		if (useMonsterBall) {
			commandList->IASetIndexBuffer(mesh_->GetIndexBufferView());// IBVを設定
		}
		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());

		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(4, viewProjection.GetWvpResource()->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(3, directionalLight_->GetLightResource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(5, pointLight_->GetLightResource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(6, spotLight_->GetLightResource()->GetGPUVirtualAddress());
		
		commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle));
		
		commandList->SetGraphicsRootDescriptorTable(1, worldTransform.GetSrvHandleGPU());
		// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
		commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices()), 1, 0, 0);
	}
}
Model* Model::LordModel(const std::string& filename) {
	Model* model = new Model();
	model->modelData_= LoadModelFile("./Resources", filename);
	//model->modelData_.material.textureFilePath=TextureManager::GetInstance()->Load(model->modelData_.material.textureFilePath);
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipeline(DirectXCommon::GetInstance()->GetDevice());
	GraphicsPipelineState::GetInstance()->CreateGraphicsPipelineSkinning(DirectXCommon::GetInstance()->GetDevice());
	for (const std::string& name:model->modelData_->names) {
		model->mesh_->SetDataVertices(static_cast<UINT>(model->modelData_->object[name].vertices.size()),name);
		model->mesh_->CreateDateResource(model->modelData_->object[name].vertices.size(),name);
		model->mesh_->CreateModelIndexResource(model->modelData_->object[name].indices.size(), name);
		// 頂点データをリソースにコピー
		std::memcpy(model->mesh_->GetData(name)
			, model->modelData_->object[name].vertices.data(),
			sizeof(Mesh::VertexData) * model->modelData_->object[name].vertices.size());
		// 頂点データをリソースにコピー
		std::memcpy(model->mesh_->GetMeshData(name).indexData
			, model->modelData_->object[name].indices.data(),
			sizeof(uint32_t) * model->modelData_->object[name].indices.size());
	}
	//model->mesh_->SetVertices(static_cast<UINT>(model->modelData_.vertices.size()));
	//model->mesh_->CreateOBJVertexResource(model->modelData_.vertices.size());
	// 頂点データをリソースにコピー
	//std::memcpy(model->mesh_->GetVertexData(), model->modelData_.vertices.data(), sizeof(Mesh::VertexData) * model->modelData_.vertices.size());
	model->material_->CreateMaterialResource();
	model->directionalLight_->CreateDirectionalLightResource();
	model->pointLight_->CreatePointLightResource();
	model->spotLight_->CreateSpotLightResource();
	return model;
	//modelData_ = LoadObjFile("resources", filename);
	//modelData_ = LoadModelFile("./Resources", filename);
}
Model::Animation* Model::LordAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	Animation* animation=new Animation();// 今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);// アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];// 最初のanimationだけ採用。複数対応予定
	animation->duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);// 時間の単位を秒に変換
	// assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報をとってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation->nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };// 右手->左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);// ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
		/*
		RotateはmNumRotationKeys/mRotationKeys,ScaleはmNumScalingKeys/mScalingKeysで取得できるので同様に行う。
		RotateはQuaternionで、右手->左手に変換するために、yとzを反転させる必要がある。Scaleはそのままでいい。
		Keyframe.value={rotate.x,-rotate.y,-rotate.z,rotate.w};
		*/
	}
	// 解析完了
	return animation;
}
void Model::Draw(WorldTransform& worldTransform, ViewProjection& viewProjection)
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Roosignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(GraphicsPipelineState::GetRootSignature());
	commandList->SetPipelineState(GraphicsPipelineState::GetPipelineState(current_blend));// PSOを設定
	commandList->IASetVertexBuffers(0, 1, mesh_->GetVertexBufferView());// VBVを設定
	if (useMonsterBall) {
		commandList->IASetIndexBuffer(mesh_->GetIndexBufferView());// IBVを設定
	}
	// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, material_->GetMaterialResource()->GetGPUVirtualAddress());

	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(4, viewProjection.GetWvpResource()->GetGPUVirtualAddress());
	// wvp用のCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(3, directionalLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(5, pointLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(6, spotLight_->GetLightResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
	commandList->SetGraphicsRootDescriptorTable(1, worldTransform.GetSrvHandleGPU());
	// 描画！(DrawCall/ドローコール)。３頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(static_cast<UINT>(mesh_->GetVertices()), 1, 0, 0);

}
Model::OBJModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 中で必要となる変数の宣言
	OBJModelData modelData; // 構築するModelData
	std::vector<Vector4>positions;// 位置
	std::vector<Vector3>normals;// 法線
	std::vector<Vector2>texcoords;// テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの
	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);// ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める
	// 3. 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			//position.y *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			//texcoord.y = texcoord.y-1.0f;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			//normal.x *= -1.0f;
			//normal.y *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			Mesh::VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのインデックスから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = elementIndices[1] > 0 ? texcoords[elementIndices[1] - 1] : Vector2{ 0, 0 };
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	// 4. ModelDataを返す
	return modelData;
}
Model::Node Model::ReadNode(aiNode* node) {
	Node result;
	Node result2;
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;// nodeのlocalMatrixを取得
	aiLocalMatrix.Transpose();// 列ベクトルを行ベクトルに転置
	for (uint32_t mindex = 0; mindex < 4; ++mindex) {
		for (uint32_t index = 0; index < 4; ++index) {
			result2.localMatrix.m[mindex][index] = aiLocalMatrix[mindex][index];
		}
	}
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);// assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = { scale.x,scale.y,scale.z };// scaleはそのまま
	result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };// x軸を反転。さらに回転方向が逆なので軸を反転させる
	result.transform.translate = { -translate.x,translate.y,translate.z };// x軸を反転
	result.localMatrix = MakeAffineMatrix(result.transform.scale, result.transform.rotate, result.transform.translate);
	result.name = node->mName.C_Str();// Node名を格納
	result.children.resize(node->mNumChildren);// 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}
Model::ModelData*  Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{

	// 1. 中で必要となる変数の宣言
	ModelData* modelData = new ModelData(); // 構築するModelData
	std::vector<Vector4>positions;// 位置
	std::vector<Vector3>normals;// 法線
	std::vector<Vector2>texcoords;// テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの
	// 2. ファイルを開く
	//std::ifstream file(directoryPath + "/" + filename);// ファイルを開く
	//assert(file.is_open());//とりあえず開けなかったら止める
	// assimp
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	//const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder|aiProcess_FlipUVs);
	assert(scene->HasMeshes());// メッシュがないのは対応しない
	// SceneのRootNodeを読んでシーン全体の階層構造を作り上げる
	modelData->rootNode = ReadNode(scene->mRootNode);
	/*各Meshの名前を格納*/
	/*注意!!名前被りは厳禁！！！*/
	///
	/// 名前被りを止める処理をここに実装
	/// 
	// 子ノードがないなら親ノード名だけ格納
	/*if (modelData->rootNode.children.empty()) {
		modelData->names.push_back(modelData->rootNode.name.c_str());
	}
	for (uint32_t NameIndex = 0; NameIndex < modelData->rootNode.children.size(); ++NameIndex) {
		modelData->names.push_back(modelData->rootNode.children[NameIndex].name.c_str());
	}*/
	// meshを解析する
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());// 法線がないMeshは今回は非対応
		//assert(mesh->HasTextureCoords(0));// TexcoordがないMeshは今回は非対応
		//modelData->object[scene->mMeshes[meshIndex]->mName.C_Str()];
		std::string meshName = mesh->mName.C_Str();
		modelData->names.push_back(meshName);
		modelData->object[meshName].useTexture = mesh->HasTextureCoords(0);

		//modelData->object[meshName].vertices.resize(mesh->mNumVertices);// 最初に頂点数分のメモリを確保しておく
		//// ここからMeshの中身(Face)の解析を行っていく
		//for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
		//	aiFace& face = mesh->mFaces[faceIndex];
		//	assert(face.mNumIndices == 3);// 三角形のみサポート
		//	// ここからFaceの中身(Vertex)の解析を行っていく
		//	for (uint32_t element = 0; element < face.mNumIndices; ++element) {
		//		uint32_t vertexIndex = face.mIndices[element];
		//		aiVector3D& position = mesh->mVertices[vertexIndex];
		//		aiVector3D& normal = mesh->mNormals[vertexIndex];
		//		//aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
		//		Mesh::VertexData vertex;
		//		vertex.position = { position.x,position.y,position.z,1.0f };
		//		vertex.normal = { normal.x,normal.y,normal.z };
		//		//vertex.texcoord = { texcoord.x,texcoord.y };
		//		// UV座標のチェックと設定
		//		if (mesh->HasTextureCoords(0)) {
		//			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
		//			vertex.texcoord = { texcoord.x, texcoord.y };
		//		} else {
		//			vertex.texcoord = { 0.0f, 0.0f }; // ダミーのUV座標
		//		}
		//		// aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対処
		//		vertex.position.x *= -1.0f;
		//		vertex.normal.x *= -1.0f;
		//		/*データを追加*/
		//		modelData->object[meshName].vertices.push_back(vertex);
		//	}
		//}
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			Mesh::VertexData vertex;
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			// UV座標のチェックと設定
			if (mesh->HasTextureCoords(0)) {
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				vertex.texcoord = { texcoord.x, texcoord.y };
			} else {
				vertex.texcoord = { 0.0f, 0.0f }; // ダミーのUV座標
			}
			vertex.position = { position.x,position.y ,position.z,1.0f };
			vertex.normal = { normal.x,normal.y ,normal.z };
			vertex.position.x *= -1.0f;
			vertex.normal.x *= -1.0f;
			/*データを追加*/
			modelData->object[meshName].vertices.push_back(vertex);
		}
		/*Index解析*/
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData->object[meshName].indices.push_back(vertexIndex);
			}
		}
		/*ボーン解析*/
		if (!mesh->mNumBones) {
			modelData->bone = false;
		} else
		{
			modelData->bone = true;
		}
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];// AssimpではJointをBoneと呼んでいる
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData->object[meshName].skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();// BindPoseMatrixに戻す
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);// 成分を抽出
			/*左手系のBindPoseMatrixを作る*/
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
				{ scale.x,scale.y,scale.z }, { rotate.x,-rotate.y,-rotate.z,rotate.w }, { -translate.x,translate.y,translate.z });
			/*InverseBindPoseMatrixにする*/
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
		// Assign material to the mesh
		uint32_t materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData->object[meshName].material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
			TextureManager::Load(directoryPath + "/" + textureFilePath.C_Str());
		}
	}
	// materialを解析する
	//for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
	//	aiMaterial* material = scene->mMaterials[materialIndex];
	//	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
	//		aiString textureFilePath;
	//		material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
	//		modelData->object[modelData->names[materialIndex-1]].material.textureFilePath  = directoryPath + "/" + textureFilePath.C_Str();
	//		
	//		//modelData->object[modelData->names[materialIndex-1]].material.textureFilePath = textureFilePath.C_Str();
	//	}
	//}
	/*for (const std::string& name : modelData->names) {
		if (modelData->object[name].material.textureFilePath == "") {
			
		}
	}*/

	return modelData;
}
Material::OBJMaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 中で必要となる変数の宣言
	Material::OBJMaterialData materialData;// 構築するMaterialData
	std::string line;// ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);;//ファイルを開く
	assert(file.is_open());
	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭識別子を読む
		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			//materialData.textureFilePath = directoryPath + "/" + textureFilename;
			//materialData.textureFilePath.push_back(directoryPath + "/" + textureFilename);
		}
	}
	// 4. MaterialDataを返す
	return materialData;
}
