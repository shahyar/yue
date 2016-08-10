// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.
//
// Helper functions to manipulate lua's stack.

#ifndef LUA_STACK_H_
#define LUA_STACK_H_

#include <tuple>

#include "lua/template_util.h"
#include "lua/types.h"

namespace lua {

// Needed by the arbitrary length version of Push.
inline void Push(State* state) {
}

// Enable push arbitrary args at the same time.
template<typename ArgType, typename... ArgTypes>
inline void Push(State* state, ArgType arg, ArgTypes... args) {
  Type<ArgType>::Push(state, arg);
  Push(state, args...);
}

// The helper function for the tuple version of Push.
template<typename Tuple, size_t... Indices>
inline void Push(State* state, const Tuple& packed,
                 internal::IndicesHolder<Indices...>) {
  Push(state, std::get<Indices>(packed)...);
}

// Treat std::tuple as unpacked args.
template<typename... ArgTypes>
inline void Push(State* state, const std::tuple<ArgTypes...>& packed) {
  Push(state, packed,
       typename internal::IndicesGenerator<sizeof...(ArgTypes)>::type());
}

// Helpers for pushing strings.
PRINTF_FORMAT(2, 3)
inline void PushFormatedString(State* state,
                               _Printf_format_string_ const char* format,
                               ...)  {
  va_list ap;
  va_start(ap, format);
  lua_pushvfstring(state, format, ap);
  va_end(ap);
}

// Needed by the arbitrary length version of toxxx.
inline bool To(State* state, int index) {
  return true;
}

// Enable getting arbitrary args at the same time.
template<typename ArgType, typename... ArgTypes>
inline bool To(State* state, int index, ArgType* arg, ArgTypes... args) {
  return Type<ArgType>::To(state, index, arg) && To(state, index + 1, args...);
}

// The helper function for the tuple version of To.
template<typename Tuple, size_t... Indices>
inline bool To(State* state, int index, Tuple* out,
               internal::IndicesHolder<Indices...>) {
  return To(state, index, &std::get<Indices>(*out)...);
}

// Get multiple values from stack.
template<typename... ArgTypes>
inline bool To(State* state, int index, std::tuple<ArgTypes...>* out) {
  return To(state, index, out,
            typename internal::IndicesGenerator<sizeof...(ArgTypes)>::type());
}

// Thin wrapper for lua_pop.
inline void Pop(State* state, size_t n) {
  lua_pop(state, n);
}

// Get the values and pop them from statck.
template<typename T>
inline bool Pop(State* state, T* result) {
  if (To(state, -Values<T>::count, result)) {
    Pop(state, Values<T>::count);
    return true;
  } else {
    return false;
  }
}

// Enable poping arbitrary args at the same time.
template<typename... ArgTypes>
inline bool Pop(State* state, ArgTypes... args) {
  if (To(state, -static_cast<int>(sizeof...(args)), args...)) {
    Pop(state, sizeof...(args));
    return true;
  } else {
    return false;
  }
}

// Thin wrappers of settop/gettop.
inline void SetTop(State* state, int index) {
  lua_settop(state, index);
}

inline int GetTop(State* state) {
  return lua_gettop(state);
}

}  // namespace lua

#endif  // LUA_STACK_H_
