#include "Entity3D.h"
#include "Entity3DCommon.h"
#include "ModelManager.h"
#include "JsonTransform.h"
#include "JsonMath.h"

void Entity3D::Init()
{
	this->camera_ = Entity3DCommon::GetInstance()->GetDefaultCamera();
	this->debugCamera_ = Entity3DCommon::GetInstance()->GetDebugCamera();
	this->cameraManager_ = Entity3DCommon::GetInstance()->GetCameraManager();
	cmdList_ = Entity3DCommon::GetInstance()->GetCmdList();
	mode_ = Entity3DCommon::GetInstance()->GetBlendMode();
	ModelResourcesSetting();

	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
}

void Entity3D::Update(const Matrix4x4* parentWorldMatrix)
{
	Matrix4x4 worldMatrix;
	Matrix4x4 localMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	// 親子関係の行列計算
	if (parentWorldMatrix) {
		worldMatrix = Multiply(localMatrix, *parentWorldMatrix);
	} else {
		worldMatrix = localMatrix;
	}

	lastCulculatedWorldMatrix_ = worldMatrix;

	// WVPMatrixを作る
	Matrix4x4 worldViewProjectionMatrix;
	// RootNode取得
	ModelData modelData = model_->GetRootNode();
	bool isDebug = cameraManager_->GetIsDebug();

	if (isDebug) {
		if (debugCamera_) {
			const Matrix4x4& viewProjectionMatrix = debugCamera_->GetViewProjectionMatrix();
			worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		} else {
			worldViewProjectionMatrix = worldMatrix;
		}
	} else {
		if (camera_) {
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		} else {
			worldViewProjectionMatrix = worldMatrix;
		}
	}

	Matrix4x4 worldInverseTransform = Inverse(worldMatrix);

	transformationMatrixData_->World = modelData.rootNode.localMatrix * worldMatrix;
	transformationMatrixData_->WVP = modelData.rootNode.localMatrix * worldViewProjectionMatrix;
	transformationMatrixData_->WorldInverseTranspose = Transpose(worldInverseTransform);;

	if (cameraData_) {
		if (isDebug) {
			if (debugCamera_) {
				cameraData_->worldPosition = debugCamera_->GetTranslate();
			}
		} else if (camera_) {
			cameraData_->worldPosition = camera_->GetTranslate();
		}
	}

	if (!isDisabled_) {
		for (auto& child : children_) {
			child->Update(&worldMatrix);
		}
	}
}

void Entity3D::Draw()
{
	Entity3DCommon::GetInstance()->SetBlendMode(mode_);
	Entity3DCommon::GetInstance()->ApplyPipeline();

	// wvp用のCBufferの場所を設定
	cmdList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(3, LightManager::GetInstance()->GetDirLightResource()->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(5, LightManager::GetInstance()->GetPointLightGroupResource()->GetGPUVirtualAddress());
	cmdList_->SetGraphicsRootConstantBufferView(6, LightManager::GetInstance()->GetSpotLightGroupResource()->GetGPUVirtualAddress());

	if (model_ && !isDisabled_) {
		model_->Draw();
	}

	// 子オブジェクトを再帰的に描画
	if (!isDisabled_) {
		for (auto& child : children_) {
			child->Draw();
		}
	}
}

json Entity3D::SaveToJson() const
{
	return json{
	   {"transform", TransformToJson(transform_)},
	   {"blendMode", static_cast<int>(mode_)},
	   {"material", ToJson(model_->GetMaterial())}
	};
}

void Entity3D::LoadFromJson(const json& j)
{
	if (j.contains("transform")) {
		TransformFromJson(j.at("transform"), transform_);
	}

	if (j.contains("blendMode")) {
		mode_ = static_cast<BlendMode>(j.at("blendMode").get<int>());
	}

	if (j.contains("material")) {
		Vector4 material{};
		FromJson(j.at("material"), material);
		SetMaterial(material);
	}
}

void Entity3D::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Entity3D::SetBlendMode(BlendMode mode)
{
	this->mode_ = mode;
	Entity3DCommon::GetInstance()->SetBlendMode(mode);
}

void Entity3D::ModelResourcesSetting()
{
	// TransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResource_ = CreateBufferResource(Graphics::GetDevice(), sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 単位行列を書きこんでおく
	transformationMatrixData_->WVP = MakeIdentity4x4();

	// カメラリソース
	cameraResource = CreateBufferResource(Graphics::GetDevice(), sizeof(CameraForGPU));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	Vector3 camPos = { 0.0f, 0.0f, 0.0f };
	if (cameraManager_->GetIsDebug()) {
		if (debugCamera_) {
			camPos = debugCamera_->GetTranslate();
		}
	} else if (camera_) {
		camPos = camera_->GetTranslate();
	}
	cameraData_->worldPosition = camPos;
}

void Entity3D::DrawImGui() {
#ifdef USE_IMGUI
	Vector4 material = model_->GetMaterial();
	ImGui::DragFloat3("Position", reinterpret_cast<float*>(&transform_.translate), 0.01f);
	ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&transform_.rotate), 0.01f);
	ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform_.scale), 0.01f, 0.0f);
	ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&material));
	ImGui::Checkbox("isDisabled", &isDisabled_);
	model_->SetMaterial(material);
#endif
}

void Entity3D::AddChild(std::unique_ptr<Entity3D> child) {
	child->SetParent(this);
	children_.push_back(std::move(child));
}

Matrix4x4 Entity3D::GetWorldMatrix() const {
	ModelData modelData = model_->GetRootNode();
	return modelData.rootNode.localMatrix * lastCulculatedWorldMatrix_;
}