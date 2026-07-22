#include "ModelManager.h"
#include "LoadLevelData.h"
#include "Editor.h"
#include "Model.h"
#include "Entity3D.h"

ModelManager* ModelManager::GetInstance()
{
	static ModelManager instance;
	return &instance;
}

void ModelManager::Shutdown()
{
	models_.clear();
	Logger::Write("ModelManager Shutdown");
}

void ModelManager::Init()
{

}

void ModelManager::LoadModel(const std::string& filePath)
{
	if (filePath.empty()) {
		Logger::Write(
			Logger::LogLevel::Error,
			"LoadMode: filePath is empty"
		);
	}

	if (models_.contains(filePath)) {
		return;
	}

	std::vector<std::string> filePaths = Split(filePath, '.');

	if (filePaths.size() < 2) {
		Logger::Write(
			Logger::LogLevel::Error,
			"Invalid model file path: " + filePath
		);
		return;
	}

	// モデル生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Init("resources/models", filePaths[0], filePaths[1]);

	// モデルをマップコンテナに格納する
	models_.insert({filePath, std::move(model)});
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// 読み込みモデルを検索
	if (models_.contains(filePath)) {
		return models_.at(filePath).get();
	}
	return nullptr;
}

std::string ModelManager::FindModelFile(const std::string& name)
{
	namespace fs = std::filesystem;
	const fs::path basePath = fs::path("resources/models") / name;
	const fs::path gltfPath = basePath / (name + ".gltf");
	const fs::path objPath = basePath / (name + ".obj");

	if (fs::exists(gltfPath)) {
		return name + ".gltf";
	}

	if (fs::exists(objPath)) {
		return name + ".obj";
	}

	// 空の文字列を返す
	return {};
}

void ModelManager::SetIsLighting(bool isLighting) {
	isModelLighting_ = isLighting;
	for (auto& model : models_) {
		if (model.second) {
			model.second->SetIsLighting(isModelLighting_);
		}
	}
}

void ModelManager::ApplyLevelData(const std::string& filePath)
{
	std::string fullPath = "resources/json/levelData/" + filePath;
	std::unique_ptr<LevelData> levelData = LoadLevelData(fullPath);

	if (!levelData) {
		Logger::Write(
			Logger::LogLevel::Error,
			"Failed to load level data: " + fullPath
		);
		return;
	}

	Editor::GetInstance()->ModelClear();

	// レベルデータからオブジェクトを生成、配置
	for (auto& obj : levelData->objects) {
		auto rootEntity = CreateEntityRecursive(obj);
		if (rootEntity) {
			Editor::GetInstance()->RegisterModel(obj.name, std::move(rootEntity));
		}
	}
}

std::vector<std::string> ModelManager::Split(std::string str, char del)
{
	std::vector<std::string> result;
	std::string subStr;

	for (const char c : str) {
		if (c == del) {
			result.push_back(subStr);
			subStr.clear();
		} else {
			subStr += c;
		}
	}

	result.push_back(subStr);
	return result;
}

std::unique_ptr<Entity3D> ModelManager::CreateEntityRecursive(const LevelData::ObjectData& obj) {
	Logger::Write("Create entity: " + obj.fileName + "," + obj.name + "," + "disbled:" + std::string(obj.disabled ? "true" : "false"));

	if (obj.fileName.empty()) {
		Logger::Write(
			Logger::LogLevel::Error,
			"fileName is empty"
		);
		return nullptr;
	}

	std::string modelNamePath = FindModelFile(obj.fileName);

	if (modelNamePath.empty()) {
		Logger::Write(
			Logger::LogLevel::Error,
			"Model file not found:" + obj.fileName
		);
		return nullptr;
	}

	LoadModel(modelNamePath);
	std::unique_ptr<Entity3D> entity = std::make_unique<Entity3D>();

	entity->Init();
	entity->SetModel(modelNamePath);
	entity->SetTranslate(obj.translation);
	entity->SetRotate(obj.rotation);
	entity->SetScale(obj.scaling);
	entity->SetName(obj.name);
	entity->SetIsDisabled(obj.disabled);

	for (const auto& childData : obj.children) {
		auto childEntity = CreateEntityRecursive(childData);
		if (childEntity) {
			entity->AddChild(std::move(childEntity));
		}
	}

	return entity;
}