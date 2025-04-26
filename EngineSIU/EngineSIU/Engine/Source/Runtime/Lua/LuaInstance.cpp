#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Classes/Components/ShapeComponent.h"
#include "Game/Data/MapManager.h"
#include "Game/Sound/SoundManager.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp, FString FilePath)
    : Env(Lua, sol::create, Lua.globals())
{
    Env["obj"] = Comp;

    BindedActor = Comp->GetOwner();

    for (const auto& Comp : BindedActor->GetComponents())
    {
        if (ShapeComp = Cast<UShapeComponent>(Comp))
        {
            break;
        }
    }

    // Load script
    ScriptFile = *FilePath;

    Lua.script_file(ScriptFile, Env);

    TickFunc = Env["Tick"];
    BeginPlayFunc = Env["BeginPlay"];
    EndPlayFunc = Env["EndPlay"];
    OnOverlapFunc = Env["OnOverlap"];
    OnEndOverlapFunc = Env["OnEndOverlap"];
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

    Lua.set_function("PlaySound", [](const std::string& name, float volume, bool loop) {
        USoundManager::Get().PlaySound(name, volume, loop);
        });
    Lua.set_function("StopSound", [](const std::string& name) {
        USoundManager::Get().StopSound(name);
        });
    Lua.set_function("SetVolume", [](const std::string& name, float volume) {
        USoundManager::Get().SetVolume(name, volume);
        });
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
        BindDelegates();
        BeginPlayFunc();
    }
}

void FLuaInstance::EndPlay()
{
    if (EndPlayFunc.valid())
    {
        EndPlayFunc();
        UnBindDelegates();
    }
}

void FLuaInstance::OnOverlap(USceneComponent* OverlappedComp)
{
    if (OnOverlapFunc.valid())
    {
        OnOverlapFunc(OverlappedComp);
    }
}

void FLuaInstance::Reload(sol::state& Lua) {
    Lua.script_file(ScriptFile, Env);
    TickFunc = Env["Tick"];
    BeginPlayFunc = Env["BeginPlay"];
    EndPlayFunc = Env["EndPlay"];
    OnOverlapFunc = Env["OnOverlap"];
    OnEndOverlapFunc = Env["OnEndOverlap"];
    LastWriteTime = std::filesystem::last_write_time(ScriptFile);
}

void FLuaInstance::BindDelegates()
{
    if (!ShapeComp)
        return;
    BeginOverlapHandle = ShapeComp->OnComponentBeginOverlap.AddDynamic(this, &FLuaInstance::OnComponentBeginOverlap);
    EndOverlapHandle = ShapeComp->OnComponentEndOverlap.AddDynamic(this, &FLuaInstance::OnComponentEndOverlap);
}

void FLuaInstance::UnBindDelegates()
{
    if (!ShapeComp)
        return;
    if (BeginOverlapHandle.IsValid())
    {
        ShapeComp->OnComponentBeginOverlap.Remove(BeginOverlapHandle);
        BeginOverlapHandle.Invalidate();
    }
    if (EndOverlapHandle.IsValid())
    {
        ShapeComp->OnComponentEndOverlap.Remove(EndOverlapHandle);
        EndOverlapHandle.Invalidate();
    }
}

void FLuaInstance::OnComponentBeginOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
    if (BindedActor)
    {
        if (OnOverlapFunc.valid())
        {
            OnOverlapFunc(OtherActor->GetRootComponent());
        }
    }
}

void FLuaInstance::OnComponentEndOverlap(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
    if (BindedActor)
    {
        if (OnEndOverlapFunc.valid())
        {
            OnEndOverlapFunc(OtherActor->GetRootComponent());
        }
    }
}
