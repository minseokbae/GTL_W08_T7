#include "LuaCompiler.h"
#include "Math/Vector.h"
#include "World/World.h"
#include "Classes/Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Engine/EditorEngine.h"
#include "Camera/CameraTransitionModifier.h"
#include "Camera/PlayerCameraManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"

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

    Lua.new_usertype<APlayerCameraManager>("PlayerCameraManager",
        sol::no_constructor,
        "AddCameraModifier", &APlayerCameraManager::AddCameraModifier
        );

    Lua.new_usertype<UCameraTransitionModifier>("CameraTransitionModifier",
        sol::constructors<>(),
        "Initialize", &UCameraTransitionModifier::Initialize);

    Lua.set_function("CreateCameraTransitionModifier", [this](USceneComponent* Comp) {
        return CreateCameraTransitionModifier(Comp);
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

    //멤버 함수는 람다로
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

    //FVector 바인딩
    Lua.new_usertype<FVector>("Vector",
        // 생성자
        sol::constructors<
        FVector(),                          // 기본 생성자
        FVector(float, float, float),       // XYZ 생성자
        FVector(float)                      // Scalar 생성자
        >(),

        // 필드
        "X", &FVector::X,
        "Y", &FVector::Y,
        "Z", &FVector::Z,

        // 연산자 바인딩
        sol::meta_function::addition, [](const FVector& a, const FVector& b) {
            return a + b;
        },

        sol::meta_function::subtraction, [](const FVector& a, const FVector& b) {
            return a - b;
        },

        sol::meta_function::multiplication, [](const FVector& v, float s) {
            return v * s;
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

    //FRotator 바인딩
    Lua.new_usertype<FRotator>("Rotator",
        //생성자
        sol::constructors<
        FRotator(),
        FRotator(float, float, float)
        >(),

        //필드
        "Roll", & FRotator::Roll,
        "Pitch", & FRotator::Pitch,
        "Yaw", & FRotator::Yaw,

        //연산자 오버로딩
        sol::meta_function::addition, [](const FRotator& a, const FRotator& b)
        {
            return a + b;
        },
        sol::meta_function::subtraction, [](const FRotator& a, const FRotator& b)
        {
            return a - b;
        },
        sol::meta_function::multiplication, sol::overload(
            [](const FRotator& rot, float scalar) {return rot * scalar; },
            [](float scalar, const FRotator& rot) {return rot * scalar; }
        ),
        sol::meta_function::unary_minus, [](const FRotator& rot)
        {
            return -rot;
        },
        sol::meta_function::to_string, [](const FRotator& rot)->std::string
        {
            std::ostringstream oss;
            oss << "Rotator (" << rot.Roll << ", " << rot.Pitch << ", " << rot.Yaw << ")";
            return oss.str();
        },

        //함수 바인딩
        "Normalize", &FRotator::Normalize,
        "GetNormalized", &FRotator::GetNormalized,
        "IsNearlyZero", &FRotator::IsNearlyZero,
        "IsZero", &FRotator::IsZero,
        "Equals", &FRotator::Equals,
        "Add", &FRotator::Add
        );

    //FString 바인딩
    Lua.new_usertype<FString>("String",
        sol::constructors<
        FString(),                      // 기본 생성자
        FString(const char*),            // const char* 생성자
        FString(const std::string&)      // std::string 생성자
        >(),

        "ToAnsiString", & FString::ToAnsiString,   // Lua에서 문자열 변환할 때
        "Len", & FString::Len,
        "IsEmpty", & FString::IsEmpty,
        sol::meta_function::to_string, [](const FString& str) {
            return str.ToAnsiString();            // Lua에서 tostring() 호출할 때
        },
        sol::meta_function::equal_to, [](const FString& a, const FString& b) {
            return a.Equals(b, ESearchCase::IgnoreCase);  // 또는 CaseSensitive
        },
        "Equals", &FString::LuaEquals
    );
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
        //Instance->second->UnBindDelegates();
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
    Lua["Global"]["CameraManager"] = CameraManager;
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

void FLuaCompiler::ChangeViewMode(int ViewModeIndex)
{
    GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->SetViewMode(static_cast<EViewModeIndex>(ViewModeIndex));
}
