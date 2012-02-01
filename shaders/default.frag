// FRAGMENT SHADER
#version 330

smooth in vec2 fg_texCoord;

uniform sampler2D msampler;
uniform vec4 tint;

out vec4 gl_FragColor;

void main()
{
  vec4 tex=texture(msampler,fg_texCoord);
  gl_FragColor=tint*tex;
}
