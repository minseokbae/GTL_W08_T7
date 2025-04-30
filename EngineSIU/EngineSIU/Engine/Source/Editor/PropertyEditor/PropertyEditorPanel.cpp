#include "PropertyEditorPanel.h"

#include <filesystem>
#include <windows.h>
#include <shellapi.h>
#include "tchar.h"

#include "World/World.h"
#include "Actors/EditorPlayer.h"
#include "Components/Light/LightComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextComponent.h"
#include "Engine/EditorEngine.h"
#include "Engine/FLoaderOBJ.h"
#include "UnrealEd/ImGuiWidget.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"
#include "Engine/Engine.h"
#include "Components/HeightFogComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"
#include "UObject/UObjectIterator.h"

#include "Renderer/Shadow/SpotLightShadowMap.h"
#include "Renderer/Shadow/PointLightShadowMap.h"
#include "Renderer/Shadow/DirectionalShadowMap.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
void PropertyEditorPanel::Render()
{
    //if (GEngine->ActiveWorld->WorldType == EWorldType::Editor)
    //{
        /* Pre Setup */
        float PanelWidth = (Width) * 0.2f - 6.0f;
        float PanelHeight = (Height) * 0.65f;

    float PanelPosX = (Width) * 0.8f + 5.0f;
    float PanelPosY = (Height) * 0.3f + 15.0f;

    ImVec2 MinSize(140, 370);
    ImVec2 MaxSize(FLT_MAX, 900);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    /* Render Start */
    ImGui::Begin("Detail", nullptr, PanelFlags);

    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
        return;
    AEditorPlayer* player = Engine->GetEditorPlayer();
    AActor* PickedActor = Engine->GetSelectedActor();
    USceneComponent* SelectedComponent = Engine->GetSelectedComponent();

    if (SelectedComponent)
    {
        ImGui::SetItemDefaultFocus();
        // TreeNode 배경색을 변경 (기본 상태)
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
            Location = SelectedComponent->GetRelativeLocation();
            Rotation = SelectedComponent->GetRelativeRotation();
            Scale = SelectedComponent->GetRelativeScale3D();

            FImGuiWidget::DrawVec3Control("Location", Location, 0, 85);
            ImGui::Spacing();

            FImGuiWidget::DrawRot3Control("Rotation", Rotation, 0, 85);
            ImGui::Spacing();

            FImGuiWidget::DrawVec3Control("Scale", Scale, 0, 85);
            ImGui::Spacing();

            SelectedComponent->SetRelativeLocation(Location);
            SelectedComponent->SetRelativeRotation(Rotation);
            SelectedComponent->SetRelativeScale3D(Scale);

            //std::string coordiButtonLabel;
            //if (player->GetCoordMode() == ECoordMode::CDM_WORLD)
            //    coordiButtonLabel = "World";
            //else if (player->GetCoordMode() == ECoordMode::CDM_LOCAL)
            //    coordiButtonLabel = "Local";

            //if (ImGui::Button(coordiButtonLabel.c_str(), ImVec2(ImGui::GetWindowContentRegionMax().x * 0.9f, 32)))
            //{
            //    player->AddCoordiMode();
            //}
            ImGui::TreePop(); // 트리 닫기
        }

        ImGui::PopStyleColor();

        static char CompSearchBuffer[128] = ""; // 검색어 입력 버퍼
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        if (ImGui::TreeNodeEx("Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
            ImGui::Text("Add");
            ImGui::SameLine();

            TArray<UClass*> CompClasses;
            GetChildOfClass(UActorComponent::StaticClass(), CompClasses);

            if (ImGui::BeginCombo("##AddComponent", "Components", ImGuiComboFlags_None))
            {
                // 1. 검색어 입력창 추가
                ImGui::InputText("Search", CompSearchBuffer, IM_ARRAYSIZE(CompSearchBuffer));
                // 2. 검색어로 필터링
                for (UClass* Class : CompClasses)
                {
                    FString ClassName = Class->GetName();
                    // 검색어가 비어있거나, 검색어가 포함된 클래스만 표시
                    if (strlen(CompSearchBuffer) == 0 || ClassName.Contains(CompSearchBuffer))
                    {
                        if (ImGui::Selectable(GetData(ClassName), false))
                        {
                            static uint32 NewCompIndex = 0;
                            USceneComponent* NewComp = Cast<USceneComponent>(
                                SelectedComponent->GetOwner()->AddComponent(
                                    Class,
                                    FString::Printf(TEXT("%s_%d"), *ClassName, NewCompIndex++)
                                )
                            );
                            if (NewComp)
                            {
                                NewComp->SetupAttachment(SelectedComponent);
                            }
                        }
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::TreePop();
        }
        ImGui::PopStyleColor();
    }

    if (PickedActor)
    {
        FString ActorTag = PickedActor->GetActorTag();
        char Tag[256];
        strcpy_s(Tag, *ActorTag);
        if (ImGui::InputText("##Actor Tag", Tag, sizeof(Tag)))
        {
            PickedActor->SetActorTag(Tag);
        }
        ImGui::SameLine();
        ImGui::Text("Actor Tag");
        namespace fs = std::filesystem;
        static char buf[256];
        FString ActorName = PickedActor->GetActorLabel();
        ActorName += ".lua";

        // 1. 처음엔 기본 ActorName으로 buf 초기화
        strcpy_s(buf, *ActorName);

        // 2. 만약 PickedActor에 이미 LuaScriptPath가 설정되어 있으면, 그걸 buf로 덮어씀
        if (PickedActor->GetLuaScriptPath().Len() != 0)
        {
            strcpy_s(buf, *PickedActor->GetLuaScriptPath());
            PickedActor->SetLuaBindState(true);
        }

        // 3. ImGui로 사용자 입력 받음
        if (ImGui::InputText("##ScriptFile", buf, sizeof(buf)))
        {
            // 입력이 바뀌었으면 그대로 LuaScriptPath 업데이트
            PickedActor->SetLuaScriptPath(buf);
            PickedActor->SetLuaBindState(true);
        }

        ImGui::SameLine();
        ImGui::Text("Script File");

        // 4. 항상 최신 buf 경로 기준으로 fs::exists 검사
        if (fs::exists(buf))
        {
            if (ImGui::Button("Edit Script"))
            {
                // 파일 열기
                std::string ansiStr = buf;
                std::vector<wchar_t> wbuf(ansiStr.size() + 1);
                MultiByteToWideChar(CP_UTF8, 0, ansiStr.c_str(), -1, wbuf.data(), (int)wbuf.size());

                LPCTSTR luaFilePath = wbuf.data();
                HINSTANCE hInst = ShellExecute(
                    NULL,
                    _T("open"),
                    luaFilePath,
                    NULL,
                    NULL,
                    SW_SHOWNORMAL
                );

                if ((INT_PTR)hInst <= 32)
                {
                    MessageBox(NULL, _T("파일 열기에 실패했습니다."), _T("Error"), MB_OK | MB_ICONERROR);
                }
            }
        }
        else
        {
            PickedActor->SetLuaBindState(false);
            if (ImGui::Button("Create Script"))
            {
                // 템플릿 복사
                fs::copy_file("template.lua", buf, fs::copy_options::overwrite_existing);
                PickedActor->SetLuaScriptPath(buf);
                PickedActor->SetLuaBindState(true);
            }
        }
    }


    if (PickedActor)
    {
        if (ImGui::Button("Duplicate"))
        {
            UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
            AActor* NewActor = Engine->ActiveWorld->DuplicateActor(Engine->GetSelectedActor());
            Engine->SelectActor(NewActor);
        }
    }

    if(SelectedComponent)
        if (UPointLightComponent* pointlightObj = Cast<UPointLightComponent>(SelectedComponent))
        {
            int pointNum = 0;
            for (const auto iter : TObjectRange<UPointLightComponent>())
            {
                if (iter != pointlightObj) {
                    pointNum++;
                }
                else {
                    break;
                }
            }

            GEngineLoop.Renderer.PointLightShadowMapPass->RenderLinearDepth(pointNum, pointlightObj);

            // Shadow Depth Map 시각화
            TArray<ID3D11ShaderResourceView*> shaderSRVs = GEngineLoop.Renderer.PointLightShadowMapPass->GetShadowViewSRVArray();
            TArray<FVector> directions = GEngineLoop.Renderer.PointLightShadowMapPass->GetDirectionArray();
            TArray<FVector> ups = GEngineLoop.Renderer.PointLightShadowMapPass->GetUpArray();

            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("PointLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return pointlightObj->GetLightColor(); },
                    [&](FLinearColor c) { pointlightObj->SetLightColor(c); });

                float Intensity = pointlightObj->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
                    pointlightObj->SetIntensity(Intensity);

                float Radius = pointlightObj->GetRadius();
                if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f")) {
                    pointlightObj->SetRadius(Radius);
                }

                // ─ Shadow Map 미리보기 (1열) ─
                ImGui::Separator();
                ImGui::Text("Shadow Maps (6 faces):");

                const int   mapsCount = shaderSRVs.Num();
                const float imgSize = 256.0f; // 원하는 크기로 조정

                for (int i = 0; i < mapsCount; ++i)
                {
                    ImGui::Text("Direction %1.f %1.f %1.f", directions[i].X, directions[i].Y, directions[i].Z);
                    ImGui::Text("Up %1.f %1.f %1.f", ups[i].X, ups[i].Y, ups[i].Z);
                    ImTextureID texID = (ImTextureID)shaderSRVs[i];
                    ImGui::Image(texID, ImVec2(imgSize, imgSize));
                    ImGui::Spacing();    // 이미지 사이에 약간의 여백
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }

    if (SelectedComponent)
        if (USpotLightComponent* SpotLightComp = Cast<USpotLightComponent>(SelectedComponent))
        {
            int spotNum = 0;
            for (const auto iter : TObjectRange<USpotLightComponent>())
            {
                if (iter != SpotLightComp) {
                    spotNum++;
                }
                else {
                    break;
                }
            }

            FEngineLoop::Renderer.SpotLightShadowMapPass->RenderLinearDepth(spotNum);

            // Shadow Depth Map 시각화
            ID3D11ShaderResourceView* shaderSRV = FEngineLoop::Renderer.SpotLightShadowMapPass->GetShadowDebugSRV();
            //FVector direction = GEngineLoop.Renderer.PointLightShadowMapPass->GetDirection();
            //FVector up = GEngineLoop.Renderer.PointLightShadowMapPass->GetUp();

            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("SpotLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty(
                    "Light Color",
                    [&] { return SpotLightComp->GetLightColor(); },
                    [&](FLinearColor c) { SpotLightComp->SetLightColor(c); }
                );

                float Intensity = SpotLightComp->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 5000.0f, "%.1f"))
                    SpotLightComp->SetIntensity(Intensity);

                float Radius = SpotLightComp->GetRadius();
                if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
                {
                    SpotLightComp->SetRadius(Radius);
                }

                LightDirection = SpotLightComp->GetDirection();
                FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);

                float OuterDegree = SpotLightComp->GetOuterDegree();
                float InnerDegree = SpotLightComp->GetInnerDegree();

                if (ImGui::SliderFloat("InnerDegree", &InnerDegree, 0.f, 90.0f, "%.1f"))
                {
                    SpotLightComp->SetInnerDegree(InnerDegree);
                    SpotLightComp->SetOuterDegree(
                        FMath::Max(InnerDegree, OuterDegree)
                    );
                }

                if (ImGui::SliderFloat("OuterDegree", &OuterDegree, 0.f, 90.f, "%.1f"))
                {
                    SpotLightComp->SetOuterDegree(OuterDegree);
                    SpotLightComp->SetInnerDegree(
                        FMath::Min(OuterDegree, InnerDegree)
                    );
                }

                // ─ Shadow Map 미리보기 (1열) ─
                ImGui::Separator();
                ImGui::Text("Testing SpotLight:");

                const float imgSize = 256.0f; // 원하는 크기로 조정

                ImGui::Text("Direction %.01f %.01f %.01f", LightDirection.X, LightDirection.Y, LightDirection.Z);
                ImTextureID texID = (ImTextureID)shaderSRV;
                ImGui::Image(texID, ImVec2(imgSize, imgSize));
                ImGui::Spacing(); // 이미지 사이에 약간의 여백

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }

    if (SelectedComponent)
        if (UDirectionalLightComponent* dirlightObj = Cast<UDirectionalLightComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            int directionalNum = 0;
            for (const auto iter : TObjectRange<UDirectionalLightComponent>())
            {
                if (iter != dirlightObj) {
                    directionalNum++;
                }
                else {
                    break;
                }
            }

            // Shadow Depth Map 시각화
            ID3D11ShaderResourceView* shaderSRV = FEngineLoop::Renderer.DirectionalShadowMap->GetShadowViewSRV(directionalNum);


            if (ImGui::TreeNodeEx("DirectionalLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return dirlightObj->GetLightColor(); },
                    [&](FLinearColor c) { dirlightObj->SetLightColor(c); });

                float Intensity = dirlightObj->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 150.0f, "%.1f"))
                    dirlightObj->SetIntensity(Intensity);

                LightDirection = dirlightObj->GetDirection();
                FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);

                // ─ Shadow Map 미리보기 (1열) ─
                ImGui::Separator();
                ImGui::Text("Directional Light Shadow SRV:");

                const float imgSize = 256.0f; // 원하는 크기로 조정

                ImGui::Text("Direction %.01f %.01f %.01f", LightDirection.X, LightDirection.Y, LightDirection.Z);
                ImTextureID texID = (ImTextureID)shaderSRV;
                ImGui::Image(texID, ImVec2(imgSize, imgSize));
                ImGui::Spacing(); // 이미지 사이에 약간의 여백

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }

    if(SelectedComponent)
        if (UAmbientLightComponent* ambientLightObj = Cast<UAmbientLightComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("AmbientLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return ambientLightObj->GetLightColor(); },
                    [&](FLinearColor c) { ambientLightObj->SetLightColor(c); });
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }

    if (SelectedComponent)
        if (UProjectileMovementComponent* ProjectileComp = Cast<UProjectileMovementComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("Projectile Movement Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                float InitialSpeed = ProjectileComp->GetInitialSpeed();
                if (ImGui::InputFloat("InitialSpeed", &InitialSpeed, 0.f, 10000.0f, "%.1f"))
                    ProjectileComp->SetInitialSpeed(InitialSpeed);

                float MaxSpeed = ProjectileComp->GetMaxSpeed();
                if (ImGui::InputFloat("MaxSpeed", &MaxSpeed, 0.f, 10000.0f, "%.1f"))
                    ProjectileComp->SetMaxSpeed(MaxSpeed);

                float Gravity = ProjectileComp->GetGravity();
                if (ImGui::InputFloat("Gravity", &Gravity, 0.f, 10000.f, "%.1f"))
                    ProjectileComp->SetGravity(Gravity); 
                
                float ProjectileLifetime = ProjectileComp->GetLifetime();
                if (ImGui::InputFloat("Lifetime", &ProjectileLifetime, 0.f, 10000.f, "%.1f"))
                    ProjectileComp->SetLifetime(ProjectileLifetime);

                FVector currentVelocity = ProjectileComp->GetVelocity();

                float velocity[3] = { currentVelocity.X, currentVelocity.Y, currentVelocity.Z };

                if (ImGui::InputFloat3("Velocity", velocity, "%.1f")) {
                    ProjectileComp->SetVelocity(FVector(velocity[0], velocity[1], velocity[2]));
                }
                
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    // TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    if (SelectedComponent)
        if (UTextComponent* textOBj = Cast<UTextComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("Text Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
            {
                if (textOBj) {
                    textOBj->SetTexture(L"Assets/Texture/font.png");
                    textOBj->SetRowColumnCount(106, 106);
                    FWString wText = textOBj->GetText();
                    int len = WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string u8Text(len, '\0');
                    WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, u8Text.data(), len, nullptr, nullptr);

                    static char buf[256];
                    strcpy_s(buf, u8Text.c_str());

                    ImGui::Text("Text: ", buf);
                    ImGui::SameLine();
                    ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
                    if (ImGui::InputText("##Text", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        textOBj->ClearText();
                        int wlen = MultiByteToWideChar(CP_UTF8, 0, buf, -1, nullptr, 0);
                        FWString newWText(wlen, L'\0');
                        MultiByteToWideChar(CP_UTF8, 0, buf, -1, newWText.data(), wlen);
                        textOBj->SetText(newWText.c_str());
                    }
                    ImGui::PopItemFlag();
                }
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }

    // TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    if (SelectedComponent)
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(SelectedComponent))
        {
            RenderForStaticMesh(StaticMeshComponent);
            RenderForMaterial(StaticMeshComponent);
        }

    if (SelectedComponent)
        if (UHeightFogComponent* FogComponent = Cast<UHeightFogComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("Exponential Height Fog", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
            {
                FLinearColor currColor = FogComponent->GetFogColor();

                float r = currColor.R;
                float g = currColor.G;
                float b = currColor.B;
                float a = currColor.A;
                float h, s, v;
                float lightColor[4] = { r, g, b, a };

                // Fog Color
                if (ImGui::ColorPicker4("##Fog Color", lightColor,
                    ImGuiColorEditFlags_DisplayRGB |
                    ImGuiColorEditFlags_NoSidePreview |
                    ImGuiColorEditFlags_NoInputs |
                    ImGuiColorEditFlags_Float))

                {

                    r = lightColor[0];
                    g = lightColor[1];
                    b = lightColor[2];
                    a = lightColor[3];
                    FogComponent->SetFogColor(FLinearColor(r, g, b, a));
                }
                RGBToHSV(r, g, b, h, s, v);
                // RGB/HSV
                bool changedRGB = false;
                bool changedHSV = false;

                // RGB
                ImGui::PushItemWidth(50.0f);
                if (ImGui::DragFloat("R##R", &r, 0.001f, 0.f, 1.f)) changedRGB = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("G##G", &g, 0.001f, 0.f, 1.f)) changedRGB = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("B##B", &b, 0.001f, 0.f, 1.f)) changedRGB = true;
                ImGui::Spacing();

                // HSV
                if (ImGui::DragFloat("H##H", &h, 0.1f, 0.f, 360)) changedHSV = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("S##S", &s, 0.001f, 0.f, 1)) changedHSV = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("V##V", &v, 0.001f, 0.f, 1)) changedHSV = true;
                ImGui::PopItemWidth();
                ImGui::Spacing();

                if (changedRGB && !changedHSV)
                {
                    // RGB -> HSV
                    RGBToHSV(r, g, b, h, s, v);
                    FogComponent->SetFogColor(FLinearColor(r, g, b, a));
                }
                else if (changedHSV && !changedRGB)
                {
                    // HSV -> RGB
                    HSVToRGB(h, s, v, r, g, b);
                    FogComponent->SetFogColor(FLinearColor(r, g, b, a));
                }

                float FogDensity = FogComponent->GetFogDensity();
                if (ImGui::SliderFloat("Density", &FogDensity, 0.00f, 3.0f))
                {
                    FogComponent->SetFogDensity(FogDensity);
                }

                float FogDistanceWeight = FogComponent->GetFogDistanceWeight();
                if (ImGui::SliderFloat("Distance Weight", &FogDistanceWeight, 0.00f, 3.0f))
                {
                    FogComponent->SetFogDistanceWeight(FogDistanceWeight);
                }

                float FogHeightFallOff = FogComponent->GetFogHeightFalloff();
                if (ImGui::SliderFloat("Height Fall Off", &FogHeightFallOff, 0.001f, 0.15f))
                {
                    FogComponent->SetFogHeightFalloff(FogHeightFallOff);
                }

                float FogStartDistance = FogComponent->GetStartDistance();
                if (ImGui::SliderFloat("Start Distance", &FogStartDistance, 0.00f, 50.0f))
                {
                    FogComponent->SetStartDistance(FogStartDistance);
                }

                float FogEndtDistance = FogComponent->GetEndDistance();
                if (ImGui::SliderFloat("End Distance", &FogEndtDistance, 0.00f, 50.0f))
                {
                    FogComponent->SetEndDistance(FogEndtDistance);
                }

                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }

    if(SelectedComponent)
        if (USphereComponent* SphereComponent = Cast<USphereComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("SphereComponent", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                float UnscaledRadius = SphereComponent->GetUnscaledSphereRadius();
                if (ImGui::InputFloat("UnscaledRadius", &UnscaledRadius, 0.f, 100.0f, "%.1f"))
                    SphereComponent->SetSphereRadius(UnscaledRadius);

                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }    
    if(SelectedComponent)
        if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("BoxComponent", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                FVector UnscaledExtent = BoxComponent->GetUnscaledBoxExtent();
                float Extent[3] = { UnscaledExtent.X, UnscaledExtent.Y, UnscaledExtent.Z };
                if (ImGui::InputFloat3("UnscaledExtent", Extent, "%.1f"))
                {
                    BoxComponent->SetBoxExtent(FVector(Extent[0], Extent[1], Extent[2]));
                }

                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }

    if (SelectedComponent)
        if (USpringArmComponent* SpringArmComponent = Cast<USpringArmComponent>(SelectedComponent))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("SpringArm Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                // 암 길이
                ImGui::DragFloat("Target Arm Length", &SpringArmComponent->TargetArmLength, 1.0f, 0.0f, 10000.0f);

                // 소켓 오프셋
                ImGui::DragFloat3("Socket Offset", &SpringArmComponent->SocketOffset.X, 1.0f);

                // 타겟 오프셋
                ImGui::DragFloat3("Target Offset", &SpringArmComponent->TargetOffset.X, 1.0f);

                // 충돌 검사
                ImGui::Checkbox("Do Collision Test", &SpringArmComponent->bDoCollisionTest);

                // 위치 랙
                ImGui::Checkbox("Enable Camera Lag", &SpringArmComponent->bEnableCameraLag);
                ImGui::DragFloat("Camera Lag Speed", &SpringArmComponent->CameraLagSpeed, 0.1f, 0.0f, 100.0f);
                ImGui::DragFloat("Camera Lag Max Dist", &SpringArmComponent->CameraLagMaxDistance, 1.0f, 0.0f, 10000.0f);

                // 회전 랙
                ImGui::Checkbox("Enable Camera Rotation Lag", &SpringArmComponent->bEnableCameraRotationLag);
                ImGui::DragFloat("Camera Rot Lag Speed", &SpringArmComponent->CameraRotationLagSpeed, 0.1f, 0.0f, 100.0f);

                // 폰 컨트롤러 회전
                ImGui::Checkbox("Use Pawn Control Rotation", &SpringArmComponent->bUsePawnControlRotation);

                // 부모 회전 상속
                ImGui::Checkbox("Inherit Pitch", &SpringArmComponent->bInheritPitch);
                ImGui::Checkbox("Inherit Yaw", &SpringArmComponent->bInheritYaw);
                ImGui::Checkbox("Inherit Roll", &SpringArmComponent->bInheritRoll);

                    ImGui::TreePop();
                }
                ImGui::PopStyleColor();
            }
        ImGui::End();
    //}
}

void PropertyEditorPanel::RGBToHSV(float r, float g, float b, float& h, float& s, float& v) const
{
    float mx = FMath::Max(r, FMath::Max(g, b));
    float mn = FMath::Min(r, FMath::Min(g, b));
    float delta = mx - mn;

    v = mx;

    if (mx == 0.0f) {
        s = 0.0f;
        h = 0.0f;
        return;
    }
    else {
        s = delta / mx;
    }

    if (delta < 1e-6) {
        h = 0.0f;
    }
    else {
        if (r >= mx) {
            h = (g - b) / delta;
        }
        else if (g >= mx) {
            h = 2.0f + (b - r) / delta;
        }
        else {
            h = 4.0f + (r - g) / delta;
        }
        h *= 60.0f;
        if (h < 0.0f) {
            h += 360.0f;
        }
    }
}

void PropertyEditorPanel::HSVToRGB(float h, float s, float v, float& r, float& g, float& b) const
{
    // h: 0~360, s:0~1, v:0~1
    float c = v * s;
    float hp = h / 60.0f;             // 0~6 구간
    float x = c * (1.0f - fabsf(fmodf(hp, 2.0f) - 1.0f));
    float m = v - c;

    if (hp < 1.0f) { r = c;  g = x;  b = 0.0f; }
    else if (hp < 2.0f) { r = x;  g = c;  b = 0.0f; }
    else if (hp < 3.0f) { r = 0.0f; g = c;  b = x; }
    else if (hp < 4.0f) { r = 0.0f; g = x;  b = c; }
    else if (hp < 5.0f) { r = x;  g = 0.0f; b = c; }
    else { r = c;  g = 0.0f; b = x; }

    r += m;  g += m;  b += m;
}

void PropertyEditorPanel::RenderForStaticMesh(UStaticMeshComponent* StaticMeshComp) const
{

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Static Mesh", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("StaticMesh");
        ImGui::SameLine();

        FString PreviewName;
        if (StaticMeshComp->GetStaticMesh() == nullptr)
        {
            PreviewName = "None";
        }
        else
        {
            PreviewName = StaticMeshComp->GetStaticMesh()->GetRenderData()->DisplayName;
        }
    
        const TMap<FName, FAssetInfo> Assets = UAssetManager::Get().GetAssetRegistry();

        static char StaticMeshSearchBuffer[128] = ""; // 검색어 입력 버퍼

        if (ImGui::BeginCombo("##StaticMesh", GetData(PreviewName), ImGuiComboFlags_None))
        {
            // 1. 검색어 입력창 추가
            ImGui::InputText("Search", StaticMeshSearchBuffer, IM_ARRAYSIZE(StaticMeshSearchBuffer));

            // 2. 검색어로 필터링
            for (const auto& Asset : Assets)
            {
                FString AssetName = Asset.Value.AssetName.ToString();
                // 검색어가 비어있거나, 검색어가 포함된 항목만 표시
                if (strlen(StaticMeshSearchBuffer) == 0 || AssetName.Contains(StaticMeshSearchBuffer))
                {
                    if (ImGui::Selectable(GetData(AssetName), false))
                    {
                        FString MeshName = Asset.Value.PackagePath.ToString() + "/" + AssetName;
                        UStaticMesh* StaticMesh = FManagerOBJ::GetStaticMesh(MeshName.ToWideString());
                        if (StaticMesh)
                        {
                            StaticMeshComp->SetStaticMesh(StaticMesh);
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}


void PropertyEditorPanel::RenderForMaterial(UStaticMeshComponent* StaticMeshComp)
{
    if (StaticMeshComp->GetStaticMesh() == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        for (uint32 i = 0; i < StaticMeshComp->GetNumMaterials(); ++i)
        {
            if (ImGui::Selectable(GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    std::cout << GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()) << std::endl;
                    SelectedMaterialIndex = i;
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }

        if (ImGui::Button("    +    ")) {
            IsCreateMaterial = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("SubMeshes", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        auto subsets = StaticMeshComp->GetStaticMesh()->GetRenderData()->MaterialSubsets;
        for (uint32 i = 0; i < subsets.Num(); ++i)
        {
            std::string temp = "subset " + std::to_string(i);
            if (ImGui::Selectable(temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    StaticMeshComp->SetselectedSubMeshIndex(i);
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }
        std::string temp = "clear subset";
        if (ImGui::Selectable(temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseDoubleClicked(0))
                StaticMeshComp->SetselectedSubMeshIndex(-1);
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();

    if (SelectedMaterialIndex != -1)
    {
        RenderMaterialView(SelectedStaticMeshComp->GetMaterial(SelectedMaterialIndex));
    }
    if (IsCreateMaterial) {
        RenderCreateMaterialView();
    }
}

void PropertyEditorPanel::RenderMaterialView(UMaterial* Material)
{
    ImGui::SetNextWindowSize(ImVec2(380, 400), ImGuiCond_Once);
    ImGui::Begin("Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    FVector MatDiffuseColor = Material->GetMaterialInfo().Diffuse;
    FVector MatSpecularColor = Material->GetMaterialInfo().Specular;
    FVector MatAmbientColor = Material->GetMaterialInfo().Ambient;
    FVector MatEmissiveColor = Material->GetMaterialInfo().Emissive;

    float dr = MatDiffuseColor.X;
    float dg = MatDiffuseColor.Y;
    float db = MatDiffuseColor.Z;
    float da = 1.0f;
    float DiffuseColorPick[4] = { dr, dg, db, da };

    ImGui::Text("Material Name |");
    ImGui::SameLine();
    ImGui::Text(*Material->GetMaterialInfo().MaterialName);
    ImGui::Separator();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", (float*)&DiffuseColorPick, BaseFlag))
    {
        FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        Material->SetDiffuse(NewColor);
    }

    float sr = MatSpecularColor.X;
    float sg = MatSpecularColor.Y;
    float sb = MatSpecularColor.Z;
    float sa = 1.0f;
    float SpecularColorPick[4] = { sr, sg, sb, sa };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", (float*)&SpecularColorPick, BaseFlag))
    {
        FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        Material->SetSpecular(NewColor);
    }


    float ar = MatAmbientColor.X;
    float ag = MatAmbientColor.Y;
    float ab = MatAmbientColor.Z;
    float aa = 1.0f;
    float AmbientColorPick[4] = { ar, ag, ab, aa };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", (float*)&AmbientColorPick, BaseFlag))
    {
        FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        Material->SetAmbient(NewColor);
    }


    float er = MatEmissiveColor.X;
    float eg = MatEmissiveColor.Y;
    float eb = MatEmissiveColor.Z;
    float ea = 1.0f;
    float EmissiveColorPick[4] = { er, eg, eb, ea };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", (float*)&EmissiveColorPick, BaseFlag))
    {
        FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        Material->SetEmissive(NewColor);
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Choose Material");
    ImGui::Spacing();

    ImGui::Text("Material Slot Name |");
    ImGui::SameLine();
    ImGui::Text(GetData(SelectedStaticMeshComp->GetMaterialSlotNames()[SelectedMaterialIndex].ToString()));

    ImGui::Text("Override Material |");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160);
    // 메테리얼 이름 목록을 const char* 배열로 변환
    std::vector<const char*> materialChars;
    for (const auto& material : FManagerOBJ::GetMaterials()) {
        materialChars.push_back(*material.Value->GetMaterialInfo().MaterialName);
    }

    //// 드롭다운 표시 (currentMaterialIndex가 범위를 벗어나지 않도록 확인)
    //if (currentMaterialIndex >= FManagerOBJ::GetMaterialNum())
    //    currentMaterialIndex = 0;

    if (ImGui::Combo("##MaterialDropdown", &CurMaterialIndex, materialChars.data(), FManagerOBJ::GetMaterialNum())) {
        UMaterial* material = FManagerOBJ::GetMaterial(materialChars[CurMaterialIndex]);
        SelectedStaticMeshComp->SetMaterial(SelectedMaterialIndex, material);
    }

    if (ImGui::Button("Close"))
    {
        SelectedMaterialIndex = -1;
        SelectedStaticMeshComp = nullptr;
    }

    ImGui::End();
}

void PropertyEditorPanel::RenderCreateMaterialView()
{
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
    ImGui::Begin("Create Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    ImGui::Text("New Name");
    ImGui::SameLine();
    static char materialName[256] = "New Material";
    // 기본 텍스트 입력 필드
    ImGui::SetNextItemWidth(128);
    if (ImGui::InputText("##NewName", materialName, IM_ARRAYSIZE(materialName))) {
        tempMaterialInfo.MaterialName = materialName;
    }

    FVector MatDiffuseColor = tempMaterialInfo.Diffuse;
    FVector MatSpecularColor = tempMaterialInfo.Specular;
    FVector MatAmbientColor = tempMaterialInfo.Ambient;
    FVector MatEmissiveColor = tempMaterialInfo.Emissive;

    float dr = MatDiffuseColor.X;
    float dg = MatDiffuseColor.Y;
    float db = MatDiffuseColor.Z;
    float da = 1.0f;
    float DiffuseColorPick[4] = { dr, dg, db, da };

    ImGui::Text("Set Property");
    ImGui::Indent();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", (float*)&DiffuseColorPick, BaseFlag))
    {
        FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        tempMaterialInfo.Diffuse = NewColor;
    }

    float sr = MatSpecularColor.X;
    float sg = MatSpecularColor.Y;
    float sb = MatSpecularColor.Z;
    float sa = 1.0f;
    float SpecularColorPick[4] = { sr, sg, sb, sa };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", (float*)&SpecularColorPick, BaseFlag))
    {
        FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        tempMaterialInfo.Specular = NewColor;
    }


    float ar = MatAmbientColor.X;
    float ag = MatAmbientColor.Y;
    float ab = MatAmbientColor.Z;
    float aa = 1.0f;
    float AmbientColorPick[4] = { ar, ag, ab, aa };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", (float*)&AmbientColorPick, BaseFlag))
    {
        FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        tempMaterialInfo.Ambient = NewColor;
    }


    float er = MatEmissiveColor.X;
    float eg = MatEmissiveColor.Y;
    float eb = MatEmissiveColor.Z;
    float ea = 1.0f;
    float EmissiveColorPick[4] = { er, eg, eb, ea };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", (float*)&EmissiveColorPick, BaseFlag))
    {
        FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        tempMaterialInfo.Emissive = NewColor;
    }
    ImGui::Unindent();

    ImGui::NewLine();
    if (ImGui::Button("Create Material")) {
        FManagerOBJ::CreateMaterial(tempMaterialInfo);
    }

    ImGui::NewLine();
    if (ImGui::Button("Close"))
    {
        IsCreateMaterial = false;
    }

    ImGui::End();
}

void PropertyEditorPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    Width = clientRect.right - clientRect.left;
    Height = clientRect.bottom - clientRect.top;
}
