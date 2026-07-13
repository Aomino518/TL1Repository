#include "PlayScene.h"
#include "SceneIncludes.h"

void PlayScene::Init()
{
    Logger::Write("現在シーンPlayScene");
   
    ImGuiManager::GetInstance()->LoadScenesJson();
}

void PlayScene::Update()
{
    /*-- 更新処理 --*/
    auto camMgr = CameraManager::GetInstance();
    if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    ImGuiManager::GetInstance()->BeginFrame();
    ImGuiManager::GetInstance()->DrawMainMenuBar();
    ImGuiManager::GetInstance()->DrawCameraWindow(camMgr);
    ImGuiManager::GetInstance()->DrawEditor();
    ImGuiManager::GetInstance()->Stats();
	ImGuiManager::GetInstance()->DrawSoundWindow();
    ImGuiManager::GetInstance()->DrawLoggerWindow();
    ImGuiManager::GetInstance()->EndFrame();
}

void PlayScene::Draw()
{
    /*-- 描画処理 --*/
    ImGuiManager::GetInstance()->Draw();
}

void PlayScene::Shutdown()
{
    Editor::GetInstance()->Clear();
}
