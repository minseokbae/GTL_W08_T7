#include "PlayerInEditorPanel.h"

#include "Engine/EditorEngine.h"
#include "World/World.h"
enum class GameState { MainMenu, Playing, GameOver };
GameState gameState = GameState::MainMenu;

void PlayerInEditorPanel::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();

    if (gameState == GameState::MainMenu) {
        ImGui::Begin("게임 메뉴", nullptr, ImGuiWindowFlags_AlwaysAutoResize);


        if (ImGui::Button("게임 시작")) {
            // 게임 시작 로직
            gameState = GameState::Playing;
            if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
            {
                EditorEngine->NewWorld();
                EditorEngine->LoadWorld("Saved/FINAL2.scene");
                EditorEngine->ActiveWorld->BeginPlay();
            }
            // 초기화 등 필요한 코드
        }
        if (ImGui::Button("처음으로 돌아가기")) {
            // 메인 메뉴로 복귀
            // 필요한 상태 초기화
        }
        ImGui::End();
    }
    else if (gameState == GameState::Playing) {
        // 인게임 HUD, 점수, 체력 등 표시
        ImGui::Begin("게임 진행 중", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("여기에 점수, 체력 등 표시");
        if (ImGui::Button("처음으로 돌아가기")) {
            gameState = GameState::MainMenu;
        }
        ImGui::End();
    }
}

void PlayerInEditorPanel::OnResize(HWND hWnd)
{
}
