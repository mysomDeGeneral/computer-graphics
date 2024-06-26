#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 baseColor;

    void main()
    {
        FragColor = texture(ourTexture, TexCoord) * vec4(baseColor, 1.0f);
    };