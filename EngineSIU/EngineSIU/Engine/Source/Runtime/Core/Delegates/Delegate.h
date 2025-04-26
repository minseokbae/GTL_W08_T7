#pragma once

#include <functional>
#include <atomic>
#include "Core/Container/Map.h"
#include "Core/Container/Array.h"
#define FUNC_DECLARE_DELEGATE(DelegateName, ReturnType, ...) \
using DelegateName = TDelegate<ReturnType(__VA_ARGS__)>;

#define FUNC_DECLARE_MULTICAST_DELEGATE(MulticastDelegateName, ReturnType, ...) \
using MulticastDelegateName = TMulticastDelegate<ReturnType(__VA_ARGS__)>;


// Unique delegate handle
class FDelegateHandle
{
    friend struct std::hash<FDelegateHandle>;
    uint64 HandleId;
    explicit FDelegateHandle(uint64 InId) : HandleId(InId) {}
    static uint64 GenerateNewID()
    {
        static std::atomic<uint64> NextHandleId{1};
        uint64 Id = NextHandleId.fetch_add(1, std::memory_order_relaxed);
        if (Id == 0)
            Id = NextHandleId.fetch_add(1, std::memory_order_relaxed);
        return Id;
    }
public:
    FDelegateHandle() : HandleId(0) {}
    static FDelegateHandle CreateHandle() { return FDelegateHandle{GenerateNewID()}; }
    bool IsValid() const { return HandleId != 0; }
    void Invalidate() { HandleId = 0; }
    bool operator==(const FDelegateHandle& Other) const { return HandleId == Other.HandleId; }
    bool operator!=(const FDelegateHandle& Other) const { return HandleId != Other.HandleId; }
};

namespace std {
template<> struct hash<FDelegateHandle>
{
    size_t operator()(FDelegateHandle InHandle) const noexcept
    {
        return std::hash<uint64>()(InHandle.HandleId);
    }
};
}

// Single-cast delegate
template<typename Signature> class TDelegate;

template<typename ReturnType, typename... ParamTypes>
class TDelegate<ReturnType(ParamTypes...)>
{
    using FuncType = std::function<ReturnType(ParamTypes...)>;
    void* CurrentInstance;
    FuncType Func;
public:
    template<typename FunctorType>
    void BindLambda(FunctorType&& InFunctor)
    {
        Func = std::forward<FunctorType>(InFunctor);
    }
    template<typename T>
    void BindDynamic(T* Obj, ReturnType(T::*Method)(ParamTypes...))
    {
        CurrentInstance = static_cast<void*>(Obj);
        Func = [Obj, Method](ParamTypes... Params) {
            (Obj->*Method)(std::forward<ParamTypes>(Params)...);
        };
    }
    template<typename T>
    void BindDynamic(T* Obj, ReturnType(T::*Method)(ParamTypes...) const)
    {
        Func = [Obj, Method](ParamTypes... Params) {
            (Obj->*Method)(std::forward<ParamTypes>(Params)...);
        };
    }
    void Unbind() { Func = nullptr; }
    bool IsBound() const { return static_cast<bool>(Func); }
    ReturnType Execute(ParamTypes... Args) const { return Func(std::forward<ParamTypes>(Args)...); }
    bool ExecuteIfBound(ParamTypes... Args) const
    {
        if (IsBound()) { Execute(std::forward<ParamTypes>(Args)...); return true; }
        return false;
    }
};

// Multi-cast delegate with object-tracking
template<typename Signature> class TMulticastDelegate;

template<typename ReturnType, typename... ParamTypes>
class TMulticastDelegate<ReturnType(ParamTypes...)>
{
    using FuncType = std::function<ReturnType(ParamTypes...)>;
    TMap<FDelegateHandle, FuncType> Delegates;
    TMap<void*, TArray<FDelegateHandle>> ObjectHandles;
public:
    // Lambda or static function
    template<typename FunctorType>
    FDelegateHandle AddLambda(FunctorType&& InFunctor)
    {
        auto Handle = FDelegateHandle::CreateHandle();
        Delegates.Add(Handle, std::forward<FunctorType>(InFunctor));
        return Handle;
    }
    // Member function
    template<typename T>
    FDelegateHandle AddDynamic(T* Obj, ReturnType(T::*Method)(ParamTypes...))
    {
        auto Handle = FDelegateHandle::CreateHandle();
        Delegates.Add(Handle, [Obj, Method](ParamTypes... Params) {
            (Obj->*Method)(std::forward<ParamTypes>(Params)...);
        });
        ObjectHandles.FindOrAdd(static_cast<void*>(Obj)).Add(Handle);
        return Handle;
    }
    template<typename T>
    FDelegateHandle AddDynamic(T* Obj, ReturnType(T::*Method)(ParamTypes...) const)
    {
        auto Handle = FDelegateHandle::CreateHandle();
        Delegates.Add(Handle, [Obj, Method](ParamTypes... Params) {
            (Obj->*Method)(std::forward<ParamTypes>(Params)...);
        });
        ObjectHandles.FindOrAdd(static_cast<void*>(Obj)).Add(Handle);
        return Handle;
    }
    // Remove a specific handle
    bool Remove(FDelegateHandle Handle)
    {
        if (Handle.IsValid())
        {
            Delegates.Remove(Handle);
            return true;
        }
        return false;
    }
    // Remove all bindings for a raw pointer object
    template<typename T>
    void RemoveAllForObject(T* Obj)
    {
        if (auto* Arr = ObjectHandles.Find(static_cast<void*>(Obj)))
        {
            for (auto& Handle : *Arr)
                Delegates.Remove(Handle);
            ObjectHandles.Remove(static_cast<void*>(Obj));
        }
    }
    // Broadcast to all
    void Broadcast(ParamTypes... Params) const
    {
        // Copy to allow safe removal during broadcast
        auto Copy = Delegates;
        for (auto& Pair : Copy)
            Pair.Value(std::forward<ParamTypes>(Params)...);
    }
};
