#include "ClearScene.h"
#include "SeekerEngine.h"
#include "SceneIncludes.h"

void ClearScene::Init()
{
	Logger::Write("現在シーンClearScene");

	ImGuiManager::GetInstance()->LoadScenesJson();
}

void ClearScene::Update()
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

void ClearScene::Draw()
{
    ImGuiManager::GetInstance()->Draw();
}

void ClearScene::Shutdown()
{
    Editor::GetInstance()->Clear();
}
