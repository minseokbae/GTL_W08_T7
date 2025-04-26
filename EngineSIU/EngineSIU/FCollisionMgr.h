#pragma once

#include "Core/Container/Array.h" 
#include "Core/Container/Set.h"   
#include "Core/Container/Map.h"   

#include <cstdint> 

class UPrimitiveComponent;
class UShapeComponent;
class AActor;

struct FOverlapPair
{
    UShapeComponent* CompA;
    UShapeComponent* CompB;

    FOverlapPair(UShapeComponent* InCompA, UShapeComponent* InCompB)
    {
        // 항상 주소값이 작은 포인터를 CompA에 저장하여 비교 일관성 확보
        if (reinterpret_cast<uintptr_t>(InCompA) > reinterpret_cast<uintptr_t>(InCompB))
        {
            CompA = InCompB;
            CompB = InCompA;
        }
        else
        {
            CompA = InCompA;
            CompB = InCompB;
        }
    }

    bool operator==(const FOverlapPair& Other) const
    {
        return CompA == Other.CompA && CompB == Other.CompB;
    }
};

namespace std
{
    template <>
    struct hash<FOverlapPair> 
    {
        std::size_t operator()(const FOverlapPair& Pair) const noexcept
        {
            std::size_t seed = 0;

            std::hash<UShapeComponent*> ptr_hasher;

            std::size_t hashA = ptr_hasher(Pair.CompA);
            seed ^= hashA + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            std::size_t hashB = ptr_hasher(Pair.CompB);
            seed ^= hashB + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed; // std::size_t 타입 반환
        }
    };

} // namespace std 마침
class FCollisionMgr
{
public:
    FCollisionMgr();
    ~FCollisionMgr();

    void RegisterComponent(UShapeComponent* Component);
    void UnregisterComponent(UShapeComponent* Component);

    // safe tick group에 넣어야 함.
    void UpdateCollisionChecks();

private:
    TArray<UShapeComponent*> RegisteredComponents;

    TSet<FOverlapPair> PreviousOverlappingPairs;

    bool IsOverlapping(UShapeComponent* CompA, UShapeComponent* CompB);

    // 필요하다면 추가적인 내부 함수들 (예: 특정 컴포넌트와 관련된 오버랩 상태 정리 함수)
    void RemoveComponentOverlapState(UShapeComponent* Component);
};



