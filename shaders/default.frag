// FRAGMENT SHADER
#version 330

in vec2 fg_texCoord;

uniform sampler2D msampler;
uniform vec4 tint;

out vec4 gl_FragColor;

void main()
{
  gl_FragColor=tint;//*texture(msampler,fg_texCoord);
}
