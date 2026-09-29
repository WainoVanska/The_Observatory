#version 330 core

in vec3 Normal;

out vec4 FragColor;

void main()
{
    vec3 normalColor = normalize(Normal) * 0.5 + 0.5;
    FragColor = vec4(normalColor, 1.0);
}
