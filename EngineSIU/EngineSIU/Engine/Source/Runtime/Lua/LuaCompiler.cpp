#include "LuaCompiler.h"
#include "Math/Vector.h"
#include "World/World.h"
#include "Classes/Components/SceneComponent.h"
#include "GameFramework/Actor.h"

FLuaCompiler::FLuaCompiler()
{
    Lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);

    Lua.new_usertype<USceneComponent>("GameObject",
        "UUID", &USceneComponent::GetUUID,
        "Location", sol::property(&USceneComponent::GetRelativeLocation, &USceneComponent::SetRelativeLocation),
        "Rotation", sol::property(&USceneComponent::GetRelativeRotation, &USceneComponent::SetRelativeRotation),
        "Scale", sol::property(&USceneComponent::GetRelativeScale3D, &USceneComponent::SetRelativeScale3D),
        "Velocity", &USceneComponent::ComponentVelocity
    );

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
