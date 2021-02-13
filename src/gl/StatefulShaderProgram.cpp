#include "StatefulShaderProgram.h"

#include <cassert>
#include <string>

#include "constants.h"
#include "shaders.h"

//
// Service types
//

namespace
{

struct UniformValueSettingVisitor final
{
public: // Atrributes

    GLint UniformLocation;

public: // Construction

    explicit UniformValueSettingVisitor(const GLint uniformLocation):
        UniformLocation(uniformLocation)
    {
        // Empty
    }

public: // Interface
    void operator()(const GLint & value) const
    {
        glUniform1i(UniformLocation, value);
    }

    void operator()(const GLuint & value) const
    {
        glUniform1ui(UniformLocation, value);
    }

    void operator()(const float & value) const
    {
        glUniform1f(UniformLocation, value);
    }

    void operator()(const glm::vec1 & value) const
    {
        glUniform1f(UniformLocation, value.x);
    }

    void operator()(const glm::vec2 & value) const
    {
        glUniform2f(UniformLocation, value.x, value.y);
    }

    void operator()(const glm::vec3 & value) const
    {
        glUniform3f(UniformLocation, value.x, value.y, value.z);
    }

    void operator()(const glm::vec4 & value) const
    {
        glUniform4f(UniformLocation, value.x, value.y, value.z, value.w);
    }
};

} // anonymous namespace

//
// Construction
//

StatefulShaderProgram::StatefulShaderProgram(UniqueShaderProgram && shaderProgram):
    m_ShaderProgram(std::move(shaderProgram))
{
    assert(m_ShaderProgram.IsSet());
}

//
// Interface
//

void StatefulShaderProgram::Use() const
{
    glUseProgram(m_ShaderProgram);
}

GLint StatefulShaderProgram::GetUniformLocation(StringView uniformName) const
{
    const auto uniformLocationIt = m_UniformLocations.find(uniformName);
    if (uniformLocationIt == m_UniformLocations.cend())
    {
        const GLint uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName.data());

        if (uniformLocation == INVALID_OPENGL_UNIFORM_LOCATION)
        {
            BOOST_LOG_TRIVIAL(error)<< "Attempted to get location for undefined uniform \"" << uniformName
                << "\" from shader program " << m_ShaderProgram;

            assert(false && "uniform must be defined in the shader program");

            return INVALID_OPENGL_UNIFORM_LOCATION;
        }

        m_UniformLocations.emplace(std::string(uniformName), uniformLocation);

        return uniformLocation;
    }

    return uniformLocationIt->second;
}

void StatefulShaderProgram::SetUniformValue(const GLint uniformLocation, const UniformValue & uniformValue)
{
    assert(IsShaderProgramCurrentlyUsed(m_ShaderProgram));

    std::visit(UniformValueSettingVisitor(uniformLocation), uniformValue);
}
