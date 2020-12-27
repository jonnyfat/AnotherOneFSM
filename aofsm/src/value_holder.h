// Copyright Yevgen
#ifndef AOFSM_SRC_VALUE_HOLDER_H_
#define AOFSM_SRC_VALUE_HOLDER_H_

namespace aofsm {
namespace internal {

// ValueHolder macht aus einem Wert von Typ ValueType einen Datentyp
template <typename ValueType, ValueType VALUE>
struct ValueHolder {
  static constexpr ValueType value = VALUE;
};

}  // namespace internal
}  // namespace aofsm

#endif  // AOFSM_SRC_VALUE_HOLDER_H_
