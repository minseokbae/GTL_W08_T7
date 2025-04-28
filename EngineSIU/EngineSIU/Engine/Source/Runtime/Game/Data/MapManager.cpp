#include "MapManager.h"
#include "Engine/Engine.h"
#include <fstream>

UMapManager& UMapManager::Get()
{
    if (UMapManager* Singleton = GEngine->MapManager)
    {
        return *Singleton;
    }
    else
    {
        UE_LOG(ELogLevel::Error, "Cannot use MapManager if no MapManagerClassName is defined!");
        assert(0);
        return *new UMapManager; // never calls this
    }
}

void UMapManager::InitMapManager(const FString& MapFilePath)
{
    std::ifstream file(*MapFilePath);
    if (!file.is_open()) {
        return;
    }
    TArray<FString> Lines;
    std::string line;
    while (std::getline(file, line)) {
        Lines.Add(FString(line));
    }
    file.close();
    ParseMap(Lines);
}

void UMapManager::ParseMap(const TArray<FString>& Lines)
{
    MapInfo.MapData.Empty();
    MapInfo.GhostStarts.Empty();
    MapInfo.Height = Lines.Num();
    MapInfo.Width = (MapInfo.Height > 0) ? Lines[0].Len() : 0;

    for (int32 Y = 0; Y < MapInfo.Height; ++Y)
    {
        TArray<ETileType> Row;
        for (int32 X = 0; X < MapInfo.Width; ++X)
        {
            TCHAR C = Lines[Y][X];
            ETileType Tile = ETileType::Empty;
            if (C == '1') Tile = ETileType::Wall;
            Row.Add(Tile);
        }
        MapInfo.MapData.Add(Row);
    }
}

ETileType UMapManager::GetTile(int32 X, int32 Y) const
{
    if (!IsValid(X, Y)) return ETileType::Empty;
    return MapInfo.MapData[Y][X];
}

bool UMapManager::IsWall(int32 X, int32 Y) const
{
    return IsValid(X, Y) && MapInfo.MapData[Y][X] == ETileType::Wall;
}

bool UMapManager::IsValid(int32 X, int32 Y) const
{
    return (X >= 0 && Y >= 0 && X < MapInfo.Width && Y < MapInfo.Height);
}
