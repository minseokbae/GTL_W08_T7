#include "LuaCompiler.h"
#include "Classes/Components/SceneComponent.h"
#include "Math/Vector.h"
#include "World/World.h"

FLuaCompiler::FLuaCompiler()
{
    Lua.open_libraries(sol::lib::base);

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

    SolEnv = sol::environment(Lua, sol::create, Lua.globals());

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
}

void FLuaCompiler::Bind(USceneComponent* Comp)
{
    BindedComp = Comp;

    Comp->BindToLua(SolEnv);

    SolEnv["obj"] = Comp;

    Lua.script_file("template.lua", SolEnv);

    TickFunc = SolEnv["Tick"];
}

void FLuaCompiler::UnBind()
{
    SolEnv["obj"] = sol::nil;
    BindedComp = nullptr;
}

void FLuaCompiler::Tick(float DeltaTime)
{
    if (!BindedComp)
        return;

    sol::object obj = SolEnv["obj"];
    if (obj.is<sol::nil_t>()) {
        std::cout << "obj is nil!" << std::endl;
    }
    else {
        std::cout << "obj is NOT nil!" << std::endl;
    }
    
    try {
        TickFunc(DeltaTime);
    }
    catch (const sol::error& e) {
        std::cout << "[Lua Error] " << e.what() << std::endl;
    }
}

void FLuaCompiler::SetTickFunc(sol::function newTick)
{
    TickFunc = newTick;
}
