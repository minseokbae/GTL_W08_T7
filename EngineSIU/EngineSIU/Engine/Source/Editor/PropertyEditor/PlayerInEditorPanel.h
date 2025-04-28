#pragma once

#include "Components/ActorComponent.h"
#include "UnrealEd/EditorPanel.h"

class PlayerInEditorPanel : public UEditorPanel
{
public:
    PlayerInEditorPanel() = default;

public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    float Width = 0, Height = 0;
    ImFont* bigFont = nullptr;
};
