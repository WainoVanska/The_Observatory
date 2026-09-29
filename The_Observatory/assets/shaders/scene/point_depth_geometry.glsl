#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;

        for (int vertex = 0; vertex < 3; ++vertex)
        {
            FragPos = gl_in[vertex].gl_Position;

            gl_Position = shadowMatrices[face] * FragPos;

            EmitVertex();
        }

        EndPrimitive();
    }
}
