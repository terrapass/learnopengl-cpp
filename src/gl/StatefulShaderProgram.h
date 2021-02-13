#pragma once

#include <variant>
#include <string_view>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "wrappers.h"

//
// Interface types
//

using UniformValue = std::variant<
    GLint,
    GLuint,
    float,
    glm::vec1,
    glm::vec2,
    glm::vec3,
    glm::vec4
>;

//
// StatefulShaderProgram
//

class StatefulShaderProgram final
{
public: // Interface types

#ifdef __cpp_lib_generic_unordered_lookup
    using StringView = std::string_view;
#else
    using StringView = const std::string &;
#endif

public: // Construction

    explicit StatefulShaderProgram(UniqueShaderProgram && shaderProgram);

    ~StatefulShaderProgram() = default;

public: // Copy / Move

    StatefulShaderProgram(const StatefulShaderProgram &) = delete;

    StatefulShaderProgram(StatefulShaderProgram &&) = default;

    StatefulShaderProgram & operator=(const StatefulShaderProgram &) = delete;

    StatefulShaderProgram & operator=(StatefulShaderProgram &&) = default;

public: // Interface

    void Use() const;

    inline GLuint Get() const;

    GLint GetUniformLocation(StringView uniformName) const;

    void SetUniformValue(const GLint uniformLocation, const UniformValue & uniformValue);

    inline void SetUniformValueByName(StringView uniformName, const UniformValue & uniformValue);

private: // Service types

    struct TransparentStringHash final
    {
        using is_transparent = std::true_type;

        std::size_t operator()(const std::string & value) const
        {
            static const std::hash<std::string> STRING_HASH;

            return STRING_HASH(value);
        }

        std::size_t operator()(std::string_view value) const
        {
            static const std::hash<std::string_view> STRING_VIEW_HASH;

            return STRING_VIEW_HASH(value);
        }
    };

private: // Members

    UniqueShaderProgram m_ShaderProgram;

    mutable std::unordered_map<std::string, GLint, TransparentStringHash, std::equal_to<>> m_UniformLocations;

    // TODO: Allow deferred uniform value setting on shader program becoming used
    // std::unordered_map<GLint, UniformValue> m_PendingUniformValueSettings;
};

//
// Interface
//

inline GLuint StatefulShaderProgram::Get() const
{
    return m_ShaderProgram;
}

inline void StatefulShaderProgram::SetUniformValueByName(StringView uniformName, const UniformValue & uniformValue)
{
    SetUniformValue(GetUniformLocation(uniformName), uniformValue);
}
