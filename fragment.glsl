#version 330 core
out vec4 FragColor;
  
in vec3 color;
in vec2 TexCoord;

uniform sampler2D bitMapTexture;

void main()
{
    FragColor = texture(bitMapTexture, TexCoord) * vec4(color, 1.0);;
}