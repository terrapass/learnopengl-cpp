#include <concepts>

#pragma once

template <typename T>
concept SizedCollection = requires(T collection) {
    typename T::value_type;
    { collection.size() } -> std::convertible_to<size_t>;
};

template <SizedCollection T>
inline size_t SizeOfCollectionData(const T & collection)
{
    return sizeof(typename T::value_type) * collection.size();
}
