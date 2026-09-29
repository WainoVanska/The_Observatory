#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;

    // Keep the real surface normal available separately (to access it on the shadow creation)
    vec3 GeometricNormal;

    mat3 TBN;
}
vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);

    mat3 normalMatrix = mat3(transpose(inverse(model)));

    vec3 N = normalize(normalMatrix * aNormal);

    vec3 T = normalize(normalMatrix * aTangent);

    // Gram-Schmidt re-orthogonalization
    T = normalize(T - dot(T, N) * N);

    vec3 B = normalize(cross(N, T));

    vs_out.FragPos = worldPosition.xyz;

    vs_out.TexCoords = aTexCoords;

    vs_out.GeometricNormal = N;

    vs_out.TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPosition;
}
