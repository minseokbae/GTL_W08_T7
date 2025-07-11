#pragma once

#include "Components/ActorComponent.h"
#include "UnrealEd/EditorPanel.h"
#include "Math/Rotator.h"

class UStaticMeshComponent;

// 헬퍼 함수 예시
template<typename Getter, typename Setter>
void DrawColorProperty(const char* label, Getter get, Setter set)
{
    ImGui::PushItemWidth(200.0f);
    FLinearColor curr = get();
    float col[4] = { curr.R, curr.G, curr.B, curr.A };

    if (ImGui::ColorEdit4(label, col,
        ImGuiColorEditFlags_DisplayRGB |
        ImGuiColorEditFlags_NoSidePreview |
        ImGuiColorEditFlags_NoInputs |
        ImGuiColorEditFlags_Float))
    {
        set(FLinearColor(col[0], col[1], col[2], col[3]));
    }
    ImGui::PopItemWidth();
}


class PropertyEditorPanel : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;


private:
    void RGBToHSV(float r, float g, float b, float& h, float& s, float& v) const;
    void HSVToRGB(float h, float s, float v, float& r, float& g, float& b) const;

    /* Static Mesh Settings */
    void RenderForStaticMesh(UStaticMeshComponent* StaticMeshComp) const;
    
    /* Materials Settings */
    void RenderForMaterial(UStaticMeshComponent* StaticMeshComp);
    void RenderMaterialView(UMaterial* Material);
    void RenderCreateMaterialView();

    void LoadBezier(float OutCurveParams[5], bool& OutbUseBezier);
    void SaveBezier(float InCurveParams[5], bool InbUseBezier);
private:
    float Width = 0, Height = 0;
    FVector Location = FVector(0, 0, 0);
    FRotator Rotation = FRotator(0, 0, 0);
    FVector Scale = FVector(0, 0, 0);
    FVector LightDirection = FVector(0, 0, 0);
    /* Material Property */
    int SelectedMaterialIndex = -1;
    int CurMaterialIndex = -1;
    UStaticMeshComponent* SelectedStaticMeshComp = nullptr;
    FObjMaterialInfo tempMaterialInfo;
    bool IsCreateMaterial;
    bool bInitialized = false;

    float CurveParams[5] = { 0.39f, 0.575f, 0.565f, 1.0f, 0 }; // x1, y1, x2, y2, presetIndex
    bool bUseBezier = false;
};
