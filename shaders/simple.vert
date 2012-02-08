// VERTEX SHADER

// A simplified shader usable only in our Tetris programs.
// We assume that InVertex is always
// (+/-0.5,+/-0.5,0)
// and that the view space is always a 10x20 aspect ratio

#version 330

in vec3 InVertex;
in vec2 InTexCoord0;
// 0-9 and 0-19
uniform vec2 offset;

smooth out vec2 fg_texCoord;

void main()
{
  // Pass through texPosition for interpolation 
  fg_texCoord=InTexCoord0;
  // Transform position
  vec4 temp;

  temp.x=-1.0f+offset.x*0.2 + (InVertex.x+0.5)/5;
  temp.y=-1.0f+offset.y*0.1 + (InVertex.y+0.5)/10;
  temp.z=0.0f;
  temp.w=1.0f;


  gl_Position=temp;
}
