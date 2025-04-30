#include "PostProcessCompositingPass.h"

#include <array>

#include "RendererHelpers.h"
#include "UnrealClient.h"
#include "Camera/PlayerCameraManager.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "Engine/EditorEngine.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "UnrealEd/EditorViewportClient.h"


FPostProcessCompositingPass::FPostProcessCompositingPass()
    : BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
    , Sampler(nullptr)
{
}

FPostProcessCompositingPass::~FPostProcessCompositingPass()
{
    
}

void FPostProcessCompositingPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManage;

    ShaderManager->AddVertexShader(L"PostProcessCompositing", L"Shaders/PostProcessCompositingShader.hlsl", "mainVS");
    ShaderManager->AddPixelShader(L"PostProcessCompositing", L"Shaders/PostProcessCompositingShader.hlsl", "mainPS");

    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    Graphics->Device->CreateSamplerState(&SamplerDesc, &Sampler);

    D3D11_BLEND_DESC BlendDesc = {};
    BlendDesc.AlphaToCoverageEnable                 = FALSE;
    BlendDesc.IndependentBlendEnable                = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    BlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Graphics->Device->CreateBlendState(&BlendDesc, &FadeBlendState);
}

void FPostProcessCompositingPass::PrepareRender()
{
}

void FPostProcessCompositingPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    // Setup
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    if (!ViewportResource)
    {
        return;
    }

    const EResourceType ResourceType = EResourceType::ERT_PostProcessCompositing; 
    FRenderTargetRHI* RenderTargetRHI = Viewport->GetViewportResource()->GetRenderTarget(ResourceType);

    ViewportResource->ClearRenderTarget(Graphics->DeviceContext, ResourceType);


    const float BlendFactor[4] = {0,0,0,0};       // 보통 0,0,0,0 사용
    const UINT SampleMask     = 0xFFFFFFFF;      
    Graphics->DeviceContext->OMSetBlendState(FadeBlendState, BlendFactor, SampleMask);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, nullptr);

    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Fog), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_PP_Fog)->SRV);

    Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &Sampler);

    Graphics->DeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    
    // Render
    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(L"PostProcessCompositing");
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(L"PostProcessCompositing");
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    BufferManager->BindConstantBuffer(TEXT("FCameraFadeConstants"), 0,EShaderStage::Pixel);
    UdpateCameraConstants();
    
    Graphics->DeviceContext->IASetInputLayout(nullptr);
    Graphics->DeviceContext->Draw(6, 0);

    // Finish
    ID3D11BlendState* NullBlend = nullptr;
    Graphics->DeviceContext->OMSetBlendState(NullBlend, BlendFactor, SampleMask);

    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    // Clear
    ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Fog), 1, NullSRV);
}

void FPostProcessCompositingPass::ClearRenderArr()
{
}

void FPostProcessCompositingPass::UdpateCameraConstants()
{
    FCameraFadeConstants CameraFadeData = {};
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine->GetGameInstance() == nullptr || EditorEngine->GetGameInstance()->GetLocalPlayer() == nullptr || EditorEngine->GetGameInstance()->GetLocalPlayer()->GetPlayerController() == nullptr)
    {
        CameraFadeData.FadeColor = FLinearColor::White;
        CameraFadeData.FadeAlpha = 0;
    }
    else
    {
        APlayerCameraManager* PCM = EditorEngine->GetGameInstance()->GetLocalPlayer()->GetPlayerController()->GetPlayerCameraManager();
    
        CameraFadeData.FadeColor = PCM->FadeColor;
        CameraFadeData.FadeAlpha = PCM->FadeAmount;
    }
    BufferManager->UpdateConstantBuffer(TEXT("FCameraFadeConstants"), CameraFadeData);

}
