// utils/_compat.hpp


#pragma once


// detect C++ version

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #define VMAFU_CPP17 1
#else
    #define VMAFU_CPP17 0
#endif

// if constexpr

#if VMAFU_CPP17
    #define VMAFU_IF_CONSTEXPR if constexpr
#else
    #define VMAFU_IF_CONSTEXPR if
#endif

// constexpr

#if VMAFU_CPP17
    #define VMAFU_CONSTEXPR constexpr
#else
    #define VMAFU_CONSTEXPR
#endif

// inline

#if VMAFU_CPP17
    #define VMAFU_INLINE inline
#else
    #define VMAFU_INLINE
#endif

// type traits

#if VMAFU_CPP17
    #define VMAFU_IS_FLOATING_POINT_V(T) std::is_floating_point_v<T>
    #define VMAFU_IS_INTEGRAL_V(T) std::is_integral_v<T>
    #define VMAFU_IS_SAME_V(T, U) std::is_same_v<T, U>
    #define VMAFU_ENABLE_IF_T(Cond) std::enable_if_t<Cond>
#else
    #define VMAFU_IS_FLOATING_POINT_V(T) std::is_floating_point<T>::value
    #define VMAFU_IS_INTEGRAL_V(T) std::is_integral<T>::value
    #define VMAFU_IS_SAME_V(T, U) std::is_same<T, U>::value
    #define VMAFU_ENABLE_IF_T(Cond) typename std::enable_if<Cond>::type
#endif

// ============ void_t ============

// #if VMAFU_CPP17
//     #define VMAFU_VOID_T std::void_t
// #else
//     template<typename...>
//     using void_t = void;
//     #define VMAFU_VOID_T void_t
// #endif

// ============ structured bindings (use tuples/structs instead) ============
// Мы НЕ эмулируем structured bindings через макросы — это опасно
// Вместо этого используем именованные структуры

// ============ deduction guides ============
// #if VMAFU_CPP17
//     #define VMAFU_DEDUCTION_GUIDE(...) __VA_ARGS__
// #else
//     #define VMAFU_DEDUCTION_GUIDE(...)
// #endif
