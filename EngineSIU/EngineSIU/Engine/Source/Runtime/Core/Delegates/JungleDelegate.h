// #include <functional>       // std::function 사용
// #include <unordered_map>    // 콜백 저장을 위한 해시맵
// #include <utility>          // std::forward 사용
// #include <cstddef>          // std::size_t 정의
//
// // 기본 템플릿 선언: 나중에 R(Args...) 형태로 특수화
// template<typename>
// class JungleDelegate;
//
// // R(Args...) 형태로 특수화된 Delegate 클래스 정의
// template<typename R, typename... Args>
// class JungleDelegate<R(Args...)>
// {
// public:
//     // 콜백 타입: 반환 R, 인자 Args...
//     using Callback = std::function<R(Args...)>;
//     // 콜백 핸들을 식별할 키 타입
//     using Handle   = std::size_t;
//
//     JungleDelegate() = default;
//     ~JungleDelegate() = default;
//
//     // 1) 함수 객체나 람다를 바인딩하고 고유 ID 반환
//     Handle Add(Callback&& cb)
//     {
//         // 새로운 ID 발급
//         Handle id = ++_currentId;
//         // 콜백을 맵에 저장
//         _callbacks.emplace(id, std::move(cb));
//         return id;
//     }
//
//     template<typename T>
//     Handle AddDynamic(T* obj, R(T::*method)(Args...))
//     {
//         // obj와 method를 캡처하여 람다로 감싸기
//         Callback cb = [obj, method](Args... args) {
//             (obj->*method)(std::forward<Args>(args)...);
//         };
//         
//         // 콜백 등록 및 핸들 발급
//         Handle id = Add(std::move(cb));
//         
//         // raw 포인터 객체 기반 핸들 추적 (객체 소멸 시 일괄 제거를 위한 포인터별 id 등록) 
//         _objectHandles[static_cast<void*>(obj)].push_back(id);
//         return id;
//     }
//
//     // 3) 객체 인스턴스의 const 멤버 함수 바인딩
//     template<typename T>
//     Handle AddDynamic(T* obj, R(T::*method)(Args...) const)
//     {
//         Callback cb = [obj, method](Args... args) {
//             (obj->*method)(std::forward<Args>(args)...);
//         };
//         Handle id = Add(std::move(cb));
//         _objectHandles[static_cast<void*>(obj)].push_back(id);
//         return id;
//     }
//
//     // 특정 핸들로 등록된 콜백 제거
//     bool Remove(Handle id)
//     {
//         return _callbacks.erase(id) > 0;
//     }
//     template<typename T>
//     void RemoveAllForObject(T* obj)
//     {
//         // raw 포인터를 키로 찾아서
//         auto it = _objectHandles.find(static_cast<void*>(obj));
//         if (it != _objectHandles.end())
//         {
//             // 그 객체가 가지고 있던 모든 핸들을 순회하며 _callbacks에서 제거
//             for (Handle id : it->second)
//             {
//                 _callbacks.erase(id);
//             }
//             _objectHandles.erase(it);
//         }
//     }
//     // 모든 리스너 호출 (멀티캐스트)
//     void Broadcast(Args&&... args) const
//     {
//         for (const auto& [id, cb] : _callbacks)
//         {
//             if (cb)
//             {
//                 cb(std::forward<Args>(args)...);
//             }
//         }
//     }
//
//     // 바인딩된 콜백이 하나도 없으면 true
//     bool IsEmpty() const { return _callbacks.empty(); }
//
//     // 간단히 바인딩 여부 확인
//     bool IsBound() const { return !IsEmpty(); }
//
//     // 바인딩된 콜백이 있을 때만 호출하는 헬퍼
//     void ExecuteIfBound(Args&&... args) const
//     {
//         if (IsBound())
//             Broadcast(std::forward<Args>(args)...);
//     }
//
//     // 모든 콜백 초기화
//     void Clear() { _callbacks.clear(); }
//
// private:
//     // 핸들 → 콜백 매핑 저장
//     std::unordered_map<Handle, Callback> _callbacks;
//     
//     // 객체 별로 발급된 핸들 목록 
//     std::unordered_map<void*, std::vector<Handle>> _objectHandles;
//     
//     // 다음에 사용할 고유 핸들 값
//     Handle _currentId{0};
// };
// // 멤버 함수를 등록햇을 때 등록한 함수의 실 객체가 지워졌으면 어떻게 대응할거지? 대응해야하나? 대응할 수 있을 까?
