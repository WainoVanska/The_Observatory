// source code saved as chars for GLSL language for fragment shader


#version 330 core
in vec3 ourColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
};