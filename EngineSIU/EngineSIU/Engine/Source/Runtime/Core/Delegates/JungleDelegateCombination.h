// #include "JungleDelegate.h"
//
// #define FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, ...) JungleDelegate<__VA_ARGS__> DelegateName
// #define FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, ...) JungleDelegate<__VA_ARGS__> DelegateName
//
// // No parameters
// #define DECLARE_JUNGLE_DELEGATE(DelegateName) FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, void())
// #define DECLARE_MULTICAST_JUNGLE_DELEGATE(DelegateName) FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, void())
//
// // One parameter
// #define DECLARE_JUNGLED_DELEGATE_OneParam(DelegateName, Param1Type)        FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, void(), Param1Type)
// #define DECLARE_MULTICAST_JUNGLE_DELEGATE_OneParam(DelegateName, Param1Type)      FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, void(), Param1Type)
//
// // Two parameters
// #define DECLARE_JUNGLE_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type)   FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, void(), Param1Type, Param2Type)
// #define DECLARE_MULTICAST_JUNGLE_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, void(), Param1Type, Param2Type)
//
// // Three parameters
// #define DECLARE_JUNGLE_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type)    FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, void(), Param1Type, Param2Type, Param3Type)
// #define DECLARE_MULTICAST_JUNGLE_DELEGATE_ThreeParams(DelegateName, Param1Type, Param2Type, Param3Type) FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, void(), Param1Type, Param2Type, Param3Type)
//
// // Four parameters
// #define DECLARE_JUNGLE_DELEGATE_FourParams(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type) \
//     FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, void(), Param1Type, Param2Type, Param3Type, Param4Type)
// #define DECLARE_MULTICAST_JUNGLE_DELEGATE_FourParams(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type) \
//     FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, void(), Param1Type, Param2Type, Param3Type, Param4Type)
//
// // Five parameters
// #define DECLARE_JUNGLE_DELEGATE_FiveParams(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
//     FUNC_JUNGLE_DECLARE_DELEGATE(DelegateName, void(), Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)
// #define DECLARE_MULTICAST_JUNGLE_DELEGATE_FiveParams(DelegateName, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
//     FUNC_JUNGLE_DECLARE_MULTICAST_DELEGATE(DelegateName, void(), Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)
