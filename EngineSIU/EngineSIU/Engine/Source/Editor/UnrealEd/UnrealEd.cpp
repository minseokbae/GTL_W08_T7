#include "UnrealEd.h"
#include "EditorPanel.h"
#include "Engine/EditorEngine.h"

#include "PropertyEditor/ControlEditorPanel.h"
#include "PropertyEditor/OutlinerEditorPanel.h"
#include "PropertyEditor/PropertyEditorPanel.h"
#include "PropertyEditor/PlayerInEditorPanel.h"

void UnrealEd::Initialize()
{
    auto ControlPanel = std::make_shared<ControlEditorPanel>();
    Panels["ControlPanel"] = ControlPanel;
    
    auto OutlinerPanel = std::make_shared<OutlinerEditorPanel>();
    Panels["OutlinerPanel"] = OutlinerPanel;
    
    auto PropertyPanel = std::make_shared<PropertyEditorPanel>();
    Panels["PropertyPanel"] = PropertyPanel;

    PIEPanel = std::make_shared<PlayerInEditorPanel>();
}

void UnrealEd::Render() const
{
    for (const auto& Panel : Panels)
    {
        Panel.Value->Render();
    }

    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine->PIEWorld)
    {
        PIEPanel->Render();
    }
}

void UnrealEd::AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

void UnrealEd::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

std::shared_ptr<UEditorPanel> UnrealEd::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
