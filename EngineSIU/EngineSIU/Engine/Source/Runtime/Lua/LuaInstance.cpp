#include "LuaInstance.h"
#include "Classes/Components/SceneComponent.h"

FLuaInstance::FLuaInstance(sol::state& Lua, USceneComponent* Comp)
    : Env(Lua, sol::create, Lua.globals())
{
    // Bind this specific object
    Env.new_usertype<USceneComponent>("GameObject",
        "UUID", &USceneComponent::GetUUID,
        "Location", sol::property(&USceneComponent::GetRelativeLocation, &USceneComponent::SetRelativeLocation),
        "Rotation", sol::property(&USceneComponent::GetRelativeRotation, &USceneComponent::SetRelativeRotation),
        "Scale", sol::property(&USceneComponent::GetRelativeScale3D, &USceneComponent::SetRelativeScale3D)
    );

    Env["obj"] = Comp;

    // Load script
    Lua.script_file("script.lua", Env);

    TickFunc = Env["Tick"];
}

void FLuaInstance::Tick(float DeltaTime)
{
    if (TickFunc.valid()) {
        TickFunc(DeltaTime);
    }
}
