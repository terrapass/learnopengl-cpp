#pragma once

#include <glad/glad.h>

#include "gl/wrappers.h"

//
// Interface types
//

struct MeshData final
{
public: // Attributes

    UniqueVao VertexArrayObject;
    GLsizei   IndicesCount;
    bool      IsIndexed;
};

//
// Mesh
//

class Mesh final
{
public: // Construction

    explicit Mesh(MeshData && data);

public: // Copy / Move

    Mesh(const Mesh &) = delete;

    Mesh(Mesh &&) = default;

    Mesh & operator=(const Mesh &) = delete;

    Mesh & operator=(Mesh &&) = default;

public: // Interface

    void Bind() const;

    void Render(const GLenum mode) const;

private: // Members

    MeshData m_Data;
};
