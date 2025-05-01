Texture2D FogTexture : register(t103);
// PostProcessing 추가 시 Texture 추가 (EShaderSRVSlot)

SamplerState CompositingSampler : register(s0);

cbuffer CameraOverlayConstants : register(b0)
{
    float4 FadeColor;
    float FadeAlpha;
    float3 Padding;

    float4 LetterBoxColor;
    
    float LetterBoxHeight;
    float LetterBoxWidth;
    float2 Padding2;

    float4 VignetteColor;
    float VignetteIntensity;
    float VignetteSmoothness;
    float2 VignetteCenter;
    
}

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_Input mainVS(uint VertexID : SV_VertexID)
{
    PS_Input Output;

    float2 QuadPositions[6] = {
        float2(-1,  1),  // Top Left
        float2(1,  1),  // Top Right
        float2(-1, -1),  // Bottom Left
        float2(1,  1),  // Top Right
        float2(1, -1),  // Bottom Right
        float2(-1, -1)   // Bottom Left
    };

    float2 UVs[6] = {
        float2(0, 0), float2(1, 0), float2(0, 1),
        float2(1, 0), float2(1, 1), float2(0, 1)
    };

    Output.Position = float4(QuadPositions[VertexID], 0, 1);
    Output.UV = UVs[VertexID];

    return Output;
}

float4 mainPS(PS_Input input) : SV_Target
{
    float2 UV = input.UV;
    float4 FogColor = FogTexture.Sample(CompositingSampler, UV);
    if (UV.x <LetterBoxWidth || UV.x > 1.0 - LetterBoxWidth || UV.y < LetterBoxHeight || UV.y > 1.0 - LetterBoxHeight)
    {
        return LetterBoxColor;
    }

    // PostProcessing Texture 추가
    // float4 Faded = lerp(FogColor, FadeColor,  FadeAlpha);

    float2 pos = UV * 2.0 - 1.0;
    float2 centered = pos - (VignetteCenter * 2.0 - 1.0);
    float dist = length(centered);
    float mask = saturate( smoothstep(1.0 - VignetteSmoothness, 1.0, dist) * VignetteIntensity );

    // 4) Combine fade alpha with mask
    float finalAlpha = FadeAlpha * mask;

    // 5) Apply fade via lerp
    float4 finalColor = lerp(FogColor, FadeColor, finalAlpha);
    return finalColor;
}
