#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
uniform vec2 offset;        //offset vec
uniform vec3 offsetcolor;   //color offset
out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos + offset, 0.0, 1.0);
    ourColor = clamp(aColor + offsetcolor, 0.0, 1.0);
}