#pragma once
#include <cmath>
#include <algorithm>
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "UObject/NameTypes.h"

// 수학 관련
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"


#define _TCHAR_DEFINED
#include <d3d11.h>

#include "UserInterface/Console.h"
#include <Math/Color.h>
#include "LightDefine.h"
#include "ShadowDefine.h"

#define GOURAUD "LIGHTING_MODEL_GOURAUD"
#define LAMBERT "LIGHTING_MODEL_LAMBERT"
#define PHONG "LIGHTING_MODEL_BLINN_PHONG"

#define PCF "SHADOW_FILTER_PDF"
#define VSM "SHADOW_FILTER_VSM"

struct FStaticMeshVertex
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
    float NormalX, NormalY, NormalZ;
    float TangentX, TangentY, TangentZ;
    float U = 0, V = 0;
    uint32 MaterialIndex;
};

// Material Subset
struct FMaterialSubset
{
    uint32 IndexStart; // Index Buffer Start pos
    uint32 IndexCount; // Index Count
    uint32 MaterialIndex; // Material Index
    FString MaterialName; // Material Name
};

struct FStaticMaterial
{
    class UMaterial* Material;
    FName MaterialSlotName;
};

// OBJ File Raw Data
struct FObjInfo
{
    FWString ObjectName; // OBJ File Name. Path + FileName.obj 
    FWString FilePath; // OBJ File Paths
    FString DisplayName; // Display Name
    FString MatName; // OBJ MTL File Name

    // Group
    uint32 NumOfGroup = 0; // token 'g' or 'o'
    TArray<FString> GroupName;

    // Vertex, UV, Normal List
    TArray<FVector> Vertices;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;

    // Faces
    TArray<int32> Faces;

    // Index
    TArray<uint32> VertexIndices;
    TArray<uint32> NormalIndices;
    TArray<uint32> UVIndices;

    // Material
    TArray<FMaterialSubset> MaterialSubsets;
};

struct FObjMaterialInfo
{
    FString MaterialName;  // newmtl : Material Name.

    uint32 TextureFlag = 0;

    bool bTransparent = false; // Has alpha channel?

    FVector Diffuse = FVector(0.7f, 0.7f, 0.7f);  // Kd : Diffuse (Vector4)
    FVector Specular = FVector(0.5f, 0.5f, 0.5f);  // Ks : Specular (Vector) 
    FVector Ambient = FVector(0.01f, 0.01f, 0.01f);   // Ka : Ambient (Vector)
    FVector Emissive = FVector::ZeroVector;  // Ke : Emissive (Vector)

    float SpecularScalar = 64.f; // Ns : Specular Power (Float)
    float DensityScalar;  // Ni : Optical Density (Float)
    float TransparencyScalar; // d or Tr  : Transparency of surface (Float)
    float BumpMultiplier;     // -bm : Bump Mulitplier ex) normalMap.xy *= BumpMultiplier; 
    uint32 IlluminanceModel; // illum: illumination Model between 0 and 10. (UINT)

    /* Texture */
    FString DiffuseTextureName;  // map_Kd : Diffuse texture
    FWString DiffuseTexturePath;

    FString AmbientTextureName;  // map_Ka : Ambient texture
    FWString AmbientTexturePath;

    FString SpecularTextureName; // map_Ks : Specular texture
    FWString SpecularTexturePath;

    FString BumpTextureName;     // map_Bump : Bump texture
    FWString BumpTexturePath;

    FString AlphaTextureName;    // map_d : Alpha texture
    FWString AlphaTexturePath;
};

// Cooked Data
namespace OBJ
{
    struct FStaticMeshRenderData
    {
        FWString ObjectName;
        FString DisplayName;

        TArray<FStaticMeshVertex> Vertices;
        TArray<UINT> Indices;

        ID3D11Buffer* VertexBuffer;
        ID3D11Buffer* IndexBuffer;

        TArray<FObjMaterialInfo> Materials;
        TArray<FMaterialSubset> MaterialSubsets;

        FVector BoundingBoxMin;
        FVector BoundingBoxMax;
    };
}

struct FVertexTexture
{
    float x, y, z;    // Position
    float u, v; // Texture
};

struct FGridParameters
{
    float GridSpacing;
    int   NumGridLines;
    FVector2D Padding1;
    
    FVector GridOrigin;
    float pad;
};

struct FSimpleVertex
{
    float dummy; // 내용은 사용되지 않음
    float padding[11];
};

struct FOBB {
    FVector4 corners[8];
};

struct FRect
{
    FRect() : TopLeftX(0), TopLeftY(0), Width(0), Height(0) {}
    FRect(float x, float y, float w, float h) : TopLeftX(x), TopLeftY(y), Width(w), Height(h) {}
    float TopLeftX, TopLeftY, Width, Height;
};

struct FPoint
{
    FPoint() : x(0), y(0) {}
    FPoint(float _x, float _y) : x(_x), y(_y) {}
    FPoint(long _x, long _y) : x(_x), y(_y) {}
    FPoint(int _x, int _y) : x(_x), y(_y) {}

    float x, y;
};

struct FBoundingBox
{
    FBoundingBox() = default;
    FBoundingBox(const FVector& _min, const FVector& _max) : min(_min), max(_max) {}
    FVector min; // Minimum extents
    float pad = 0.0f;
    FVector max; // Maximum extents
    float pad1 = 0.0f;
    bool Intersect(const FVector& rayOrigin, const FVector& rayDir, float& outDistance) const
    {
        float tmin = -FLT_MAX;
        float tmax = FLT_MAX;
        constexpr float epsilon = 1e-6f;

        // X축 처리
        if (fabs(rayDir.X) < epsilon)
        {
            // 레이가 X축 방향으로 거의 평행한 경우,
            // 원점의 x가 박스 [min.X, max.X] 범위 밖이면 교차 없음
            if (rayOrigin.X < min.X || rayOrigin.X > max.X)
                return false;
        }
        else
        {
            float t1 = (min.X - rayOrigin.X) / rayDir.X;
            float t2 = (max.X - rayOrigin.X) / rayDir.X;
            if (t1 > t2)  std::swap(t1, t2);

            // tmin은 "현재까지의 교차 구간 중 가장 큰 min"
            tmin = (t1 > tmin) ? t1 : tmin;
            // tmax는 "현재까지의 교차 구간 중 가장 작은 max"
            tmax = (t2 < tmax) ? t2 : tmax;
            if (tmin > tmax)
                return false;
        }

        // Y축 처리
        if (fabs(rayDir.Y) < epsilon)
        {
            if (rayOrigin.Y < min.Y || rayOrigin.Y > max.Y)
                return false;
        }
        else
        {
            float t1 = (min.Y - rayOrigin.Y) / rayDir.Y;
            float t2 = (max.Y - rayOrigin.Y) / rayDir.Y;
            if (t1 > t2)  std::swap(t1, t2);

            tmin = (t1 > tmin) ? t1 : tmin;
            tmax = (t2 < tmax) ? t2 : tmax;
            if (tmin > tmax)
                return false;
        }

        // Z축 처리
        if (fabs(rayDir.Z) < epsilon)
        {
            if (rayOrigin.Z < min.Z || rayOrigin.Z > max.Z)
                return false;
        }
        else
        {
            float t1 = (min.Z - rayOrigin.Z) / rayDir.Z;
            float t2 = (max.Z - rayOrigin.Z) / rayDir.Z;
            if (t1 > t2)  std::swap(t1, t2);

            tmin = (t1 > tmin) ? t1 : tmin;
            tmax = (t2 < tmax) ? t2 : tmax;
            if (tmin > tmax)
                return false;
        }

        // 여기까지 왔으면 교차 구간 [tmin, tmax]가 유효하다.
        // tmax < 0 이면, 레이가 박스 뒤쪽에서 교차하므로 화면상 보기엔 교차 안 한다고 볼 수 있음
        if (tmax < 0.0f)
            return false;

        // outDistance = tmin이 0보다 크면 그게 레이가 처음으로 박스를 만나는 지점
        // 만약 tmin < 0 이면, 레이의 시작점이 박스 내부에 있다는 의미이므로, 거리를 0으로 처리해도 됨.
        outDistance = (tmin >= 0.0f) ? tmin : 0.0f;

        return true;
    }


    FBoundingBox TransformWorld(const FMatrix& worldMatrix) const
    {
        // 로컬 공간에서 중심과 half-extents 계산
        FVector center = (min + max) * 0.5f;
        FVector extents = (max - min) * 0.5f;

        // 중심을 월드 공간으로 변환
        FVector worldCenter = worldMatrix.TransformPosition(center);

        // 행렬의 상위 3x3 부분의 절대값 벡터를 사용하여 각 축에 대한 확장량 계산
        // (FVector::GetAbs()와 DotProduct() 함수가 사용됨)
        FVector row0(worldMatrix.M[0][0], worldMatrix.M[0][1], worldMatrix.M[0][2]);
        FVector row1(worldMatrix.M[1][0], worldMatrix.M[1][1], worldMatrix.M[1][2]);
        FVector row2(worldMatrix.M[2][0], worldMatrix.M[2][1], worldMatrix.M[2][2]);

        row0 = FVector::GetAbs(row0);
        row1 = FVector::GetAbs(row1);
        row2 = FVector::GetAbs(row2);

        float worldExtentX = row0.Dot(extents);
        float worldExtentY = row1.Dot(extents);
        float worldExtentZ = row2.Dot(extents);

        FVector worldExtents(worldExtentX, worldExtentY, worldExtentZ);

        return FBoundingBox(worldCenter - worldExtents, worldCenter + worldExtents);
    }
    void Expand(const FVector& point)
    {
        min.X = std::min(min.X, point.X);
        min.Y = std::min(min.Y, point.Y);
        min.Z = std::min(min.Z, point.Z);

        max.X = std::max(max.X, point.X);
        max.Y = std::max(max.Y, point.Y);
        max.Z = std::max(max.Z, point.Z);
    }

    FVector GetCorner(int index) const
    {
        return FVector(
            (index & 1) ? max.X : min.X,
            (index & 2) ? max.Y : min.Y,
            (index & 4) ? max.Z : min.Z
        );
    }

};

struct FCone
{
    FVector ConeApex; // 원뿔의 꼭짓점
    float ConeRadius; // 원뿔 밑면 반지름

    FVector ConeBaseCenter; // 원뿔 밑면 중심
    float ConeHeight; // 원뿔 높이 (Apex와 BaseCenter 간 차이)
    
    FVector4 Color;

    int ConeSegmentCount; // 원뿔 밑면 분할 수
    float pad[3];
};

struct FSphere
{
    FVector Center;
    float Radius;
};
struct FBox
{
    FVector Center;
    float pad0;

    FVector Extent;
    float pad1;
};

struct FPrimitiveCounts
{
    int BoundingBoxCount;
    int ConeCount;
    int OBBCount;
    int SphereCount;

    int BoxCount;
    float pad[3];
};

#define MAX_LIGHTS 16
enum ELightType {
    POINT_LIGHT = 1,
    SPOT_LIGHT = 2,
    DIRECTIONAL_LIGHT = 3,
    AMBIENT_LIGHT = 4,
    NUM_LIGHT_TYPES = 5
};

struct FMaterialConstants
{
    FVector DiffuseColor;
    float TransparencyScalar;

    FVector SpecularColor;
    float SpecularScalar;

    FVector EmissiveColor;
    float DensityScalar;

    FVector AmbientColor;
    uint32 TextureFlag;
};

struct FObjectConstantBuffer
{
    FMatrix WorldMatrix;
    FMatrix InverseTransposedWorld;
    
    FVector4 UUIDColor;
    
    int bIsSelected;
    FVector pad;
};

struct FCameraConstantBuffer
{
    FMatrix ViewMatrix;
    FMatrix InvViewMatrix;
    
    FMatrix ProjectionMatrix;
    FMatrix InvProjectionMatrix;
    
    FVector ViewLocation;
    float Padding1;

    float NearClip;
    float FarClip;
    FVector2D Padding2;
};

struct FSubUVConstant
{
    FVector2D uvOffset;
    FVector2D uvScale;
};

struct FLitUnlitConstants
{
    int bIsLit; // 1 = Lit, 0 = Unlit 
    FVector pad;
};

struct FViewModeConstants
{
    uint32 ViewMode;
    FVector pad;
};

struct FSubMeshConstants
{
    float bIsSelectedSubMesh;
    FVector pad;
};

struct FTextureUVConstants
{
    float UOffset;
    float VOffset;
    float pad0;
    float pad1;
};

struct FLinePrimitiveBatchArgs
{
    FGridParameters GridParam;
    ID3D11Buffer* VertexBuffer;
    int BoundingBoxCount;
    int ConeCount;
    int ConeSegmentCount;
    int OBBCount;

    int SphereCount;
    int BoxCount;
};

struct FViewportSize
{
    FVector2D ViewportSize;
    FVector2D Padding;
};

struct FVertexInfo
{
    uint32_t NumVertices;
    uint32_t Stride;
    ID3D11Buffer* VertexBuffer;
};

struct FIndexInfo
{
    uint32_t NumIndices;
    ID3D11Buffer* IndexBuffer;
};

struct FBufferInfo
{
    FVertexInfo VertexInfo;
    FIndexInfo IndexInfo;
};

struct FScreenConstants
{
    FVector2D ScreenSize;   // 화면 전체 크기 (w, h)
    FVector2D UVOffset;     // 뷰포트 시작 UV (x/sw, y/sh)
    FVector2D UVScale;      // 뷰포트 크기 비율 (w/sw, h/sh)
    FVector2D Padding;      // 정렬용 (사용 안 해도 무방)
};

struct FFogConstants
{
    FLinearColor FogColor;
    
    float StartDistance;
    float EndDistance;    
    float FogHeight;
    float FogHeightFalloff;
    
    float FogDensity;
    float FogDistanceWeight;
    float padding1;
    float padding2;
};
struct FCameraOverlayConstants
{
    FLinearColor FadeColor;
    float FadeAlpha;
    FVector Padding;

    FLinearColor LetterBoxColor;
    float LetterBoxHeight;
    float LetterBoxWidth;
    FVector2D Padding2;

    FLinearColor VignetteColor;
    float  VignetteIntensity;   // 비네팅 강도 (0 = 없음, 1 = 최대)
    float  VignetteSmoothness;  // 비네팅 부드러움 (0.01 ~ 0.5 정도)
    FVector2D VignetteCenter;   // 비네팅 중심 (0.5,0.5 = 화면 중앙)
};
