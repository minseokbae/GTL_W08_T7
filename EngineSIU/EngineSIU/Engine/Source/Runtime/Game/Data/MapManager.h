#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

enum class ETileType : uint8
{
    Empty = 0,
    Wall = 1
};

struct FMapInfo
{
    int32 Width = 0;
    int32 Height = 0;
    TArray<TArray<ETileType>> MapData;
};

class UMapManager : public UObject
{
    DECLARE_CLASS(UMapManager, UObject)

private:
    FMapInfo MapInfo;

public:
    UMapManager() = default;

    /** UMapManager를 가져옵니다. (싱글톤) */
    static UMapManager& Get();

    /** 맵 파일을 로드하여 초기화합니다. */
    void InitMapManager(const FString& MapFilePath);

    /** 맵 정보 조회 */
    ETileType GetTile(int32 X, int32 Y) const;
    bool IsWall(int32 X, int32 Y) const;
    bool IsValid(int32 X, int32 Y) const;
    int32 GetWidth() const { return MapInfo.Width; }
    int32 GetHeight() const { return MapInfo.Height; }
    const TArray<TArray<ETileType>>& GetMapData() const { return MapInfo.MapData; }

private:
    void ParseMap(const TArray<FString>& Lines);
};
