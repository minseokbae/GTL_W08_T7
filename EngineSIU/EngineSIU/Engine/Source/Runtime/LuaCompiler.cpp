#include "LuaCompiler.h"
#include "Classes/Components/SceneComponent.h"
#include "Math/Vector.h"

FLuaCompiler::FLuaCompiler()
{
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
        }
    );
}

void FLuaCompiler::Bind(USceneComponent* Comp)
{
    Comp->BindToLua(Lua);
}

void FLuaCompiler::Tick(float DeltaTime)
{
    Lua.open_libraries(sol::lib::base);
    Lua.script_file("script.lua");
    sol::function TickFunc = Lua["Tick"];
    TickFunc(DeltaTime);
}
