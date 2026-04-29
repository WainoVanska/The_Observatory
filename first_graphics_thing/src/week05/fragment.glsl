#version 330 core

in vec3 ourColor;
in vec2 ourTexCoord;
out vec4 FragColor;

uniform sampler2D texture1;     // looks inside texture1 at UV coordinates ourTexCoord, get pixel from there and use as a color

void main()
{
    FragColor = texture(texture1, ourTexCoord);
}