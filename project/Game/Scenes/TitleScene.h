#pragma once
#include "SceneIncludes.h"
#include "BaseScene.h"
#include "Particle2DEmitter.h"

class TitleScene : public BaseScene
{
public:
	// 初期化
	void Init() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	void Shutdown() override;

	const char* GetSceneName() const override { return "TITLE"; }

private:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> spr_monsterBall;
	std::unique_ptr<Particle2DEmitter> emitter_;
};