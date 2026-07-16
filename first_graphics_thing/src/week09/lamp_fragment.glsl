#version 330 core

out vec4 FragColor;

uniform vec3 lampColor;

void main()
{
    // Convert to linear space before writing it into the linear HDR framebuffer
    vec3 linearLampColor = pow(
        lampColor,
        vec3(2.2)
    );

    const float intensity = 2.0;

    FragColor = vec4(
        linearLampColor * intensity,
        1.0
    );
}