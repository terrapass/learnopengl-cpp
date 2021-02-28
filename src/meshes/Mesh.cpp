#include "Mesh.h"

#include <cassert>

#include "gl/utils.h"

//
// Construction
//

Mesh::Mesh(MeshData && data):
    m_Data(std::move(data))
{
    assert(m_Data.VertexArrayObject.IsSet());
}

//
// Interface
//

void Mesh::Bind() const
{
    glBindVertexArray(m_Data.VertexArrayObject);
}

void Mesh::Render(const GLenum mode) const
{
    assert(GetBoundVertexArray() == m_Data.VertexArrayObject && "Mesh must be bound before rendering");

    if (m_Data.IsIndexed)
        glDrawElements(mode, m_Data.IndicesCount, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(mode, 0, m_Data.IndicesCount);
}
