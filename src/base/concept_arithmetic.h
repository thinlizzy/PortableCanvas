#ifndef PORTABLE_CANVAS_ARITHMETIC_2025_01_04_H
#define PORTABLE_CANVAS_ARITHMETIC_2025_01_04_H

#include <concepts>
#include <type_traits>

namespace pc {

template<typename T>
concept arithmetic = std::is_arithmetic_v<T>;

}

#endif
