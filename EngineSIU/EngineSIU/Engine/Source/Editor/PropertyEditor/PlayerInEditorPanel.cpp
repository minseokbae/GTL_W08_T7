#include "PlayerInEditorPanel.h"

#include "Engine/EditorEngine.h"
#include "World/World.h"
#include "Engine/Classes/Engine/GameInstance.h"

enum class GameState { MainMenu, Playing, PauseMenu, GameOver };
GameState gameState = GameState::MainMenu;

void PlayerInEditorPanel::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);

    // 게임 중 ESC로 일시정지 메뉴(PauseMenu) 띄우기
    if (gameState == GameState::Playing && (GetAsyncKeyState(VK_ESCAPE) & 0x1)) {
        gameState = GameState::PauseMenu;
        if (Engine->GetCurrentController() == Engine->GetEditorPlayer())
            Engine->SetCurrentController(Engine->GetGameInstance()->GetLocalPlayer()->GetPlayerController());
        else
        {
            Engine->SetCurrentController(Engine->GetEditorPlayer());
        }
    }

    // PauseMenu ESC로 닫기
    if (gameState == GameState::PauseMenu && (GetAsyncKeyState(VK_ESCAPE) & 0x1)) {
        gameState = GameState::Playing;
        if (Engine->GetCurrentController() == Engine->GetEditorPlayer())
            Engine->SetCurrentController(Engine->GetGameInstance()->GetLocalPlayer()->GetPlayerController());
        else
        {
            Engine->SetCurrentController(Engine->GetEditorPlayer());
        }
    }

    // PauseMenu
    if (gameState == GameState::PauseMenu) {
        ImVec2 windowSize(300, 120);
        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(ImVec2(center.x - windowSize.x * 0.5f, center.y - windowSize.y * 0.5f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

        ImGui::Begin("PauseMenu", nullptr, flags);

        ImGui::Spacing(); ImGui::Spacing();

        float buttonWidth = 200.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float offset = (avail - buttonWidth) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
        if (ImGui::Button("재시작", ImVec2(buttonWidth, 0))) {
            if (Engine) {
                Engine->EndPIE();
                Engine->NewWorld();
                Engine->LoadWorld("Saved/FINAL2.scene");
                Engine->StartPIE();
                gameState = GameState::Playing;
            }
        }

        ImGui::Spacing();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
        if (ImGui::Button("처음으로 돌아가기", ImVec2(buttonWidth, 0))) {
            if (Engine) {
                Engine->EndPIE();
                Engine->NewWorld();
                Engine->LoadWorld("Saved/MainMenu.scene");
                Engine->StartPIE();
                gameState = GameState::MainMenu;
            }
        }

        ImGui::End();
        return; // PauseMenu가 뜨면 아래 UI는 그리지 않음
    }

    // 메인 메뉴
    if (gameState == GameState::MainMenu) {
        ImVec2 windowSize(400, 120);
        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(ImVec2(center.x - windowSize.x * 0.5f, center.y - windowSize.y * 0.5f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;

        ImGui::Begin("게임 메뉴", nullptr, flags);

        ImGui::Spacing(); ImGui::Spacing();

        float buttonWidth = 200.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float offset = (avail - buttonWidth) * 0.5f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

        if (ImGui::Button("게임 시작", ImVec2(buttonWidth, 0))) {
            if (Engine) {
                Engine->EndPIE();
                Engine->NewWorld();
                Engine->LoadWorld("Saved/FINAL2.scene");
                Engine->StartPIE();
                gameState = GameState::Playing;
            }
        }
        ImGui::End();
    }
    // 인게임 HUD
    else if (gameState == GameState::Playing) {
        ImGui::Begin("게임 진행 중", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("코인 : %.0f/5", Cast<UEditorEngine>(GEngine)->ActiveWorld->GetGameMode()->GetScore());
        ImGui::End();
    }
}

void PlayerInEditorPanel::OnResize(HWND hWnd)
{
}
