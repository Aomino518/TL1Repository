#pragma once
#include <stdlib.h>
#include <string>
#include <memory>
#include <unordered_map>

enum class InspectorCategory {
    None,
    Sprite,
    Model,
    Particle,
    Particle2D,
    Camera,
    DirectionalLight,
    PointLight,
    SpotLight,
    WorldField,
    WorldField2D
};

struct InspectorSelection {
    InspectorCategory category = InspectorCategory::None;
    std::string name;
};

class Sprite;
class Entity3D;
class ParticleEmitter;
class Particle2DEmitter;
class Editor
{
public:
    static Editor* GetInstance();

    void Update();

    void Draw3D();

	void Draw();

    // 登録
    void RegisterSprite(const std::string& name, Sprite* sprite);
    void RegisterModel(const std::string& name, std::unique_ptr<Entity3D> model);
    void RegisterParticle(const std::string& name);
    void RegisterParticle2D(const std::string& name);

    // 保存と読み込み
    void SaveSceneJson(const std::string& path) const;
    void LoadSceneJson(const std::string& path);
    void ClearSceneJson(const std::string& path);

    void Clear();
    void ModelClear();

private:
    Editor() = default;
    ~Editor() = default;
    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;

	void DrawHierarchy();
	void DrawInspector();
    
    // Entity3Dの親子構造で再帰表示する
    void DrawEntityHierarchy(Entity3D* entity);

    InspectorSelection selection_;

    // Hierarchyで現在選択されているEntity3D
    Entity3D* selectedEntity_ = nullptr;

    std::unordered_map<std::string, Sprite*> sprites_;
    std::unordered_map<std::string, std::unique_ptr<Entity3D>> models_;
    std::unordered_map<std::string, ParticleEmitter*> particles_;
    std::unordered_map<std::string, Particle2DEmitter*> particles2D_;
};