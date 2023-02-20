#version 330 core
out vec4 FragColor;

in vec3 colour;
in vec2 texCoord;

uniform sampler2D tex;

void main()
{
  FragColor = texture(tex, texCoord) * vec4(colour, 0.5);
}