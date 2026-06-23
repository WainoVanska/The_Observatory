#version 330 core

out vec4 FragColor;

uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;

    return (2.0 * nearPlane * farPlane) /
           (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    float depth = linearizeDepth(gl_FragCoord.z);

    // This is only for visualization.
    // Your room is small, so 8.0 gives better contrast than 100.0.
    float debugFar = 8.0;

    float normalizedDepth = clamp(depth / debugFar, 0.0, 1.0);

    // near = white, far = black
    float brightness = 1.0 - normalizedDepth;

    FragColor = vec4(vec3(brightness), 1.0);
}