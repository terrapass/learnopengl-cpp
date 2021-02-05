#pragma once

#include <type_traits>
#include <optional>
#include <cassert>
#include <vector>

#include <boost/log/trivial.hpp>

#include "traits.h"

namespace detail
{

template <typename T>
struct HasCreateManyStaticMethod final
{
private: // Compile-time service

    template <typename S, typename = std::enable_if_t<!std::is_member_function_pointer_v<decltype(&S::CreateMany)>>>
    static std::true_type test(int);

    template <typename>
    static std::false_type test(...);

public: // Constants

    static constexpr const bool value = decltype(test<T>(0))::value;
};

} // namespace detail

template <typename Traits>
class UniqueEntity final
{
    static_assert(std::is_integral_v<typename Traits::ValueType>, "UniqueEntity is intended to wrap integer values");

public: // Construction / Destruction

    UniqueEntity() = default;

    ~UniqueEntity()
    {
        Reset();
    }

private: // Construction

    explicit UniqueEntity(const typename Traits::ValueType value):
        m_Value(value)
    {
        assert(m_Value.has_value());

        BOOST_LOG_TRIVIAL(trace)<< "Created unique " << Traits::ValueTypeDisplayName << ' ' << *m_Value;
    }

public: // Copy / Move

    UniqueEntity(const UniqueEntity &) = delete;

    UniqueEntity(UniqueEntity && other):
        m_Value(std::move(other.m_Value))
    {
        other.m_Value.reset();

        assert(m_Value.has_value());
        assert(!other.m_Value.has_value());
    }

    UniqueEntity & operator=(const UniqueEntity &) = delete;

    UniqueEntity & operator=(UniqueEntity && other)
    {
        Reset();

        assert(!m_Value.has_value());

        m_Value.swap(other.m_Value);

        assert(!other.m_Value.has_value());
    }

public: // Factory methods

    template <typename... Args>
    static UniqueEntity Create(Args &&... args)
    {
        return UniqueEntity(Traits::Create(std::forward<Args>(args)...));
    }

    template <typename T = Traits, typename... Args>
    static std::enable_if_t<detail::HasCreateManyStaticMethod<T>::value, std::vector<UniqueEntity>> CreateMany(
        const size_t count,
        Args &&...   args
    )
    {
        const std::vector<typename Traits::ValueType> rawResult = Traits::CreateMany(count, std::forward<Args>(args)...);
        assert(rawResult.size() == count);

        std::vector<UniqueEntity> result;
        result.reserve(rawResult.size());

        for (const typename Traits::ValueType value : rawResult)
            result.push_back(UniqueEntity(value));

        return result;
    }

    template <typename T = Traits, typename... Args>
    static std::enable_if_t<!detail::HasCreateManyStaticMethod<T>::value, std::vector<UniqueEntity>> CreateMany(
        const size_t count,
        Args &&...   args
    )
    {
        std::vector<UniqueEntity> result;
        result.reserve(count);

        for (size_t i = 0; i < count; i++)
            result.push_back(UniqueEntity(Traits::Create(std::forward<Args>(args)...)));

        return result;
    }

public: // Implicit conversion

    operator typename Traits::ValueType() const
    {
        assert(m_Value.has_value() && "UniqueEntity must be initialized at this point");

        return *m_Value;
    }

public: // Interface

    bool IsSet() const
    {
        return m_Value.has_value();
    }

    void Reset()
    {
        if (!m_Value.has_value())
            return;

        Traits::Destroy(*m_Value);

        BOOST_LOG_TRIVIAL(trace)<< "Destroyed unique " << Traits::ValueTypeDisplayName << ' ' << *m_Value;

        m_Value.reset();
    }

private: // Members

    std::optional<typename Traits::ValueType> m_Value;
};

//
// Type aliases
//

using UniqueVAO    = UniqueEntity<detail::VaoTraits>;
using UniqueBuffer = UniqueEntity<detail::BufferTraits>;
using UniqueShader = UniqueEntity<detail::ShaderTraits>;
using UniqueShaderProgram = UniqueEntity<detail::ShaderProgramTraits>;

// TEMP
static_assert(detail::HasCreateManyStaticMethod<detail::VaoTraits>::value);
static_assert(detail::HasCreateManyStaticMethod<detail::BufferTraits>::value);
static_assert(!detail::HasCreateManyStaticMethod<detail::ShaderTraits>::value);
static_assert(!detail::HasCreateManyStaticMethod<detail::ShaderProgramTraits>::value);
// END TEMP
