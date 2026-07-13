#include "GameOverScene.h"
#include "SeekerEngine.h"
#include "SceneIncludes.h"

void GameOverScene::Init()
{
	Logger::Write("現在シーンGameOverScene");

	ImGuiManager::GetInstance()->LoadScenesJson();
}

void GameOverScene::Update()
{
	auto camMgr = CameraManager::GetInstance();

    ImGuiManager::GetInstance()->BeginFrame();
    ImGuiManager::GetInstance()->DrawMainMenuBar();
    ImGuiManager::GetInstance()->DrawCameraWindow(camMgr);
    ImGuiManager::GetInstance()->DrawEditor();
    ImGuiManager::GetInstance()->Stats();
    ImGuiManager::GetInstance()->DrawSoundWindow();
    ImGuiManager::GetInstance()->DrawLoggerWindow();
    ImGuiManager::GetInstance()->EndFrame();
}

void GameOverScene::Draw()
{
    ImGuiManager::GetInstance()->Draw();
}

void GameOverScene::Shutdown()
{
    Editor::GetInstance()->Clear();
}
