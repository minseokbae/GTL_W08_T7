#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"
#include "Game/Data/MapManager.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath)
    : Env(Lua, sol::create, Lua.globals())
{
    Env["obj"] = Comp;

    // Load script
    ScriptFile = *FilePath;

    Lua.script_file(ScriptFile, Env);

    TickFunc = Env["Tick"];
    BeginPlayFunc = Env["BeginPlay"];
    EndPlayFunc = Env["EndPlay"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);

    const auto& MapData = UMapManager::Get().GetMapData();
    sol::table luaMap = Lua.create_table();
    for (int y = 0; y < MapData.Num(); ++y)
    {
        sol::table row = Lua.create_table();
        for (int x = 0; x < MapData[y].Num(); ++x)
            row[x + 1] = static_cast<int>(MapData[y][x]);
        luaMap[y + 1] = row;
    }
    Lua["gameMap"] = luaMap;
}

void FLuaInstance::Tick(float DeltaTime)
{
    if (TickFunc.valid()) 
    {
        TickFunc(DeltaTime);
    }
}

void FLuaInstance::BeginPlay()
{
    if (BeginPlayFunc.valid())
    {
        BeginPlayFunc();
    }
}

void FLuaInstance::EndPlay()
{
    if (EndPlayFunc.valid())
    {
        EndPlayFunc();
    }
}

void FLuaInstance::Reload(sol::state& Lua) {
    Lua.script_file(ScriptFile, Env);
    TickFunc = Env["Tick"];
    BeginPlayFunc = Env["BeginPlay"];
    EndPlayFunc = Env["EndPlay"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);
}
