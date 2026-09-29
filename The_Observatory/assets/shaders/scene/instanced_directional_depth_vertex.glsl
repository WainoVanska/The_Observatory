#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 systemModel;
uniform mat4 lightSpaceMatrix;

void main()
{
    mat4 worldMatrix = systemModel * instanceMatrix;

    gl_Position = lightSpaceMatrix * worldMatrix * vec4(aPos, 1.0);
}
