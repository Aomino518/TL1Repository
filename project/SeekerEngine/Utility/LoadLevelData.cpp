#include "LoadLevelData.h"
#include <fstream>
#include <assert.h>

std::unique_ptr<LevelData> LoadLevelData(const std::string& fullPath)
{
	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullPath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name =
		deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンスを生成
	std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

	// 多めにメモリを確保
	levelData->objects.reserve(100);

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		LevelData::ObjectData rootObject = ParseObject(object);
		levelData->objects.push_back(rootObject);
	}

	return levelData;
}

LevelData::ObjectData ParseObject(const nlohmann::json& object)
{
    LevelData::ObjectData objectData{};

	if (object.contains("name") && object["name"].is_string()) {
		objectData.name = object["name"].get<std::string>();
	}

    if (object.contains("file_name")) {
        objectData.fileName = object["file_name"];
    }

	if (object.contains("disabled")) {
		objectData.disabled = object["disabled"];
	}

	if (objectData.name.empty()) {
		objectData.name = objectData.fileName;
	}

    // transform読み込み
    const nlohmann::json& transform = object["transform"];
    objectData.translation.x = (float)transform["translation"][0];
    objectData.translation.y = (float)transform["translation"][2];
    objectData.translation.z = (float)transform["translation"][1];
    
    objectData.rotation.x = -(float)transform["rotation"][0];
    objectData.rotation.y = -(float)transform["rotation"][2];
    objectData.rotation.z = -(float)transform["rotation"][1];

    objectData.scaling.x = (float)transform["scaling"][0];
    objectData.scaling.y = (float)transform["scaling"][2];
    objectData.scaling.z = (float)transform["scaling"][1];

    if (object.contains("children")) {
        for (const auto& childJson : object["children"]) {
            LevelData::ObjectData childObject = ParseObject(childJson);
            objectData.children.push_back(childObject);
        }
    }

    return objectData;
}
