#include "LuaCompiler.h"
#include "Math/Vector.h"
#include "World/World.h"
#include "Classes/Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/EditorEngine.h"
#include "Camera/CameraTransitionModifier.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/PlayerCameraManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"
#include "EngineStatics.h"
#include "Runtime/Game/Sound/SoundManager.h"

FLuaCompiler::FLuaCompiler()
{
    Lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string, sol::lib::os);

    Lua.new_usertype<USceneComponent>("GameObject",
        "UUID", sol::property(&USceneComponent::GetUUID),
        "Location", sol::property(&USceneComponent::GetRelativeLocation, &USceneComponent::SetRelativeLocation),
        "Rotation", sol::property(&USceneComponent::GetRelativeRotation, &USceneComponent::SetRelativeRotation),
        "Scale", sol::property(&USceneComponent::GetRelativeScale3D, &USceneComponent::SetRelativeScale3D),
        "Velocity", &USceneComponent::ComponentVelocity,
        "Tag", sol::property(&USceneComponent::GetTag),
        "Destroy", &USceneComponent::DestroyOwner
    );

    // PlayerCameraManager 바인딩
    Lua.new_usertype<APlayerCameraManager>("PlayerCameraManager",
        sol::no_constructor,
        "AddCameraModifier", &APlayerCameraManager::AddCameraModifier,
        "GetCachedCamera", &APlayerCameraManager::GetCachedCamera
    );

    Lua.new_usertype<UCameraTransitionModifier>("CameraTransitionModifier",
        sol::constructors<>(),
        "Initialize", &UCameraTransitionModifier::Initialize);

    // FCameraShakePattern 바인딩 (간소화)
    Lua.new_usertype<FCameraShakePattern>("ShakePattern",
        "RotationAmplitudeMultiplier", &FCameraShakePattern::RotationAmplitudeMultiplier,
        "RotationFrequencyMultiplier", &FCameraShakePattern::RotationFrequencyMultiplier,
        "RotationAmplitude", &FCameraShakePattern::RotationAmplitude
    );

    // UCameraShakeBase 바인딩 (간소화)
    Lua.new_usertype<UCameraShakeBase>("CameraShake",
        sol::no_constructor,
        "PlayShake", &UCameraShakeBase::PlayShake,
        "StopShake", &UCameraShakeBase::StopShake,
        "Duration", &UCameraShakeBase::Duration,
        "BlendInTime", &UCameraShakeBase::BlendInTime,
        "BlendOutTime", &UCameraShakeBase::BlendOutTime,
        "ShakePattern", &UCameraShakeBase::ShakePattern
    );

    Lua.set_function("CreateCameraTransitionModifier", [this](USceneComponent* Comp) {
        return CreateCameraTransitionModifier(Comp);
        });

    // CreateCameraShake 함수 바인딩
    Lua.set_function("CreateCameraShake", [this](float Duration, float BlendInTime, float BlendOutTime) -> UCameraShakeBase* {
        try {
            UCameraShakeBase* result = CreateCameraShake(Duration, BlendInTime, BlendOutTime);
            UE_LOG(ELogLevel::Display, "CreateCameraShake: Created shake with duration %f", Duration);
            return result;
        }
        catch (const std::exception& e) {
            UE_LOG(ELogLevel::Error, "CreateCameraShake: Exception occurred: %s", e.what());
            return nullptr;
        }
        catch (...) {
            UE_LOG(ELogLevel::Error, "CreateCameraShake: Unknown exception occurred");
            return nullptr;
        }
        });

    Lua["Global"] = Lua.create_table();

    Lua.set_function("print", [&Lua = this->Lua](sol::variadic_args args) {
        std::ostringstream oss;
        for (auto arg : args) {
            sol::object obj = arg.get<sol::object>();
            sol::function tostring = Lua["tostring"]; // 안전한 tostring
            sol::object str = tostring(obj);

            if (str.is<std::string>())
                oss << str.as<std::string>() << " ";
            else
                oss << "[non-string] ";
        }

        FString LuaLog = oss.str().c_str();
        UE_LOG(ELogLevel::Display, TEXT("%s"), *LuaLog);
        });

    // 멤버 함수 바인딩
    Lua.set_function("AddScore", [this](float Score) {
        AddScore(Score);
        });

    Lua.set_function("GameOver", [this]() {
        GameOver();
        });

    Lua.set_function("ChangeViewMode", [this](int index) {
        ChangeViewMode(index);
        });

    Input = Lua.create_table();
    Lua["Input"] = Input;

    // FVector 바인딩
    Lua.new_usertype<FVector>("Vector",
        sol::constructors<
        FVector(),
        FVector(float, float, float),
        FVector(float)
        >(),
        "X", &FVector::X,
        "Y", &FVector::Y,
        "Z", &FVector::Z,
        sol::meta_function::addition, [](const FVector& a, const FVector& b) {
            return a + b;
        },
        sol::meta_function::subtraction, [](const FVector& a, const FVector& b) {
            return a - b;
        },
        sol::meta_function::multiplication, sol::overload(
            [](const FVector& v, float s) { return v * s; },
            [](float s, const FVector& v) { return v * s; }
        ),
        sol::meta_function::to_string, [](const FVector& v) -> std::string {
            std::ostringstream oss;
            oss << "Vector(" << v.X << ", " << v.Y << ", " << v.Z << ")";
            return oss.str();
        }
    );

    // FRotator 바인딩
    Lua.new_usertype<FRotator>("Rotator",
        sol::constructors<
        FRotator(),
        FRotator(float, float, float)
        >(),
        "Pitch", &FRotator::Pitch,
        "Yaw", &FRotator::Yaw,
        "Roll", &FRotator::Roll,
        sol::meta_function::addition, [](const FRotator& a, const FRotator& b) {
            return a + b;
        },
        sol::meta_function::subtraction, [](const FRotator& a, const FRotator& b) {
            return a - b;
        },
        sol::meta_function::multiplication, sol::overload(
            [](const FRotator& rot, float scalar) {return rot * scalar; },
            [](float scalar, const FRotator& rot) {return rot * scalar; }
        ),
        sol::meta_function::unary_minus, [](const FRotator& rot) {
            return -rot;
        },
        sol::meta_function::to_string, [](const FRotator& rot)->std::string {
            std::ostringstream oss;
            oss << "Rotator (" << rot.Roll << ", " << rot.Pitch << ", " << rot.Yaw << ")";
            return oss.str();
        },
        "Normalize", &FRotator::Normalize,
        "GetNormalized", &FRotator::GetNormalized,
        "IsNearlyZero", &FRotator::IsNearlyZero,
        "IsZero", &FRotator::IsZero,
        "Equals", &FRotator::Equals,
        "Add", &FRotator::Add
    );

    // FString 바인딩
    Lua.new_usertype<FString>("String",
        sol::constructors<
        FString(),
        FString(const char*),
        FString(const std::string&)
        >(),
        "ToAnsiString", &FString::ToAnsiString,
        "Len", &FString::Len,
        "IsEmpty", &FString::IsEmpty,
        sol::meta_function::to_string, [](const FString& str) {
            return str.ToAnsiString();
        },
        sol::meta_function::equal_to, [](const FString& a, const FString& b) {
            return a.Equals(b, ESearchCase::IgnoreCase);
        },
        "Equals", &FString::LuaEquals
    );

    // 사운드 관련 함수 바인딩
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

void FLuaCompiler::Bind(AActor* Actor)
{
    uint32 UUID = Actor->GetUUID();

    auto Instance = LuaInstances.find(UUID);
    if (Instance != LuaInstances.end())
        LuaInstances.erase(Instance);

    LuaInstances[UUID] = std::make_unique<FLuaInstance>(Lua, Actor->GetRootComponent(), Actor->GetLuaScriptPath());
}

void FLuaCompiler::UnBind(AActor* Actor)
{
    uint32 UUID = Actor->GetUUID();

    auto Instance = LuaInstances.find(UUID);
    if (Instance != LuaInstances.end())
        LuaInstances.erase(Instance);
}

void FLuaCompiler::BeginPlay()
{
    for (auto& Instance : LuaInstances)
    {
        Instance.second->BeginPlay();
    }
}

void FLuaCompiler::EndPlay()
{
    for (auto& Instance : LuaInstances)
    {
        Instance.second->EndPlay();
    }
}

void FLuaCompiler::Tick(float DeltaTime)
{
    UpdateInput();
    for (auto& Instance : LuaInstances)
    {
        auto curTime = std::filesystem::last_write_time(Instance.second->GetScriptFile());
        if (curTime != Instance.second->GetLastWriteTime())
        {
            Instance.second->Reload(Lua);
        }
        Instance.second->UpdateGlobal(Lua);
        Instance.second->Tick(DeltaTime);
    }
}

void FLuaCompiler::UpdateInput()
{
    bool A = (::GetAsyncKeyState('A') & 0x8000) != 0;
    bool S = (::GetAsyncKeyState('S') & 0x8000) != 0;
    bool D = (::GetAsyncKeyState('D') & 0x8000) != 0;
    bool W = (::GetAsyncKeyState('W') & 0x8000) != 0;
    Input.set("A", A);
    Input.set("S", S);
    Input.set("D", D);
    Input.set("W", W);
}

void FLuaCompiler::AddPlayerCameraMangerToLua(APlayerCameraManager* CameraManager)
{
    if (CameraManager == nullptr) {
        UE_LOG(ELogLevel::Error, "AddPlayerCameraMangerToLua: CameraManager is null!");
        return;
    }

    UE_LOG(ELogLevel::Display, "AddPlayerCameraMangerToLua: Setting CameraManager in Lua Global table");
    Lua["Global"]["CameraManager"] = CameraManager;

    sol::object storedManager = Lua["Global"]["CameraManager"];
    if (storedManager.is<APlayerCameraManager*>()) {
        UE_LOG(ELogLevel::Display, "AddPlayerCameraMangerToLua: CameraManager successfully set in Global table");
    }
    else {
        UE_LOG(ELogLevel::Error, "AddPlayerCameraMangerToLua: Failed to set CameraManager in Global table");
    }
}

void FLuaCompiler::UpdateGlobal(std::unique_ptr<FLuaInstance> LuaInstance)
{
    LuaInstance->UpdateGlobal(Lua);
}

void FLuaCompiler::AddScore(float score)
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    AGameMode* GameMode = Engine->ActiveWorld->GetGameMode();
    GameMode->AddScore(score);
}

void FLuaCompiler::GameOver()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    AGameMode* GameMode = Engine->ActiveWorld->GetGameMode();
    GameMode->GameOver();
}

UCameraTransitionModifier* FLuaCompiler::CreateCameraTransitionModifier(USceneComponent* Comp)
{
    return FObjectFactory::ConstructObject<UCameraTransitionModifier>(Comp);
}

UCameraShakeBase* FLuaCompiler::CreateCameraShake(float Duration, float BlendInTime, float BlendOutTime)
{
    UE_LOG(ELogLevel::Display, "CreateCameraShake: Creating camera shake (Duration: %f, BlendIn: %f, BlendOut: %f)",
        Duration, BlendInTime, BlendOutTime);

    UCameraShakeBase* ShakeObj = nullptr;

    try {
        ShakeObj = FObjectFactory::ConstructObject<UCameraShakeBase>(nullptr);

        if (ShakeObj) {
            UE_LOG(ELogLevel::Display, "CreateCameraShake: Successfully created camera shake object");

            // 기본 속성 설정
            ShakeObj->Duration = Duration;
            ShakeObj->BlendInTime = BlendInTime;
            ShakeObj->BlendOutTime = BlendOutTime;

            // 기본 쉐이크 패턴 설정
            ShakeObj->ShakePattern.RotationAmplitudeMultiplier = 1.0f;
            ShakeObj->ShakePattern.RotationFrequencyMultiplier = 10.0f;
            ShakeObj->ShakePattern.RotationAmplitude = FVector(1.0f, 1.0f, 1.0f);

            // 노이즈 생성기 초기화
            ShakeObj->InitializeNoiseGenerator();

            UE_LOG(ELogLevel::Display, "CreateCameraShake: Shake pattern configured");
        }
        else {
            UE_LOG(ELogLevel::Error, "CreateCameraShake: Failed to create camera shake object");
        }
    }
    catch (const std::exception& e) {
        UE_LOG(ELogLevel::Error, "CreateCameraShake: Exception occurred: %s", e.what());
        return nullptr;
    }
    catch (...) {
        UE_LOG(ELogLevel::Error, "CreateCameraShake: Unknown exception occurred");
        return nullptr;
    }

    return ShakeObj;
}

void FLuaCompiler::ChangeViewMode(int ViewModeIndex)
{
    GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->SetViewMode(static_cast<EViewModeIndex>(ViewModeIndex));
}
