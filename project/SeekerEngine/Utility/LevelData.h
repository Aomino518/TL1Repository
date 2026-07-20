#pragma once
#include <string>
#include <vector>
#include "Vector3.h"

class LevelData {
public:
	struct ObjectData {
		std::string name; // Hierarchy上のエンティティ名
		std::string fileName; // 読み込むモデルファイル名
		Vector3 translation;
		Vector3 rotation;
		Vector3 scaling;
		std::vector<ObjectData> children;
	};

	std::vector<ObjectData> objects;
};