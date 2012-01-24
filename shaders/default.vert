// VERTEX SHADER
#version 330

in vec3 InVertex;
in vec2 InTexCoord0;

uniform mat4 Projection;
uniform mat4 Modelview;

smooth out vec2 fg_texCoord;

void main()
{
  // Pass through texPosition for interpolation 
  fg_texCoord=InTexCoord0;
  // Transform position
  vec4 temp;
  temp.xyz=InVertex;
  temp.w=1.0f;
  gl_Position=(Projection*Modelview)*temp;
}
