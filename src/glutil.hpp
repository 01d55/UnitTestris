#ifndef GLUTIL_HPP
#define GLUTIL_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


// Print OpenGL error. If no error, no print.
void printGlError()
{
  GLenum error = glGetError();

  switch(error)
    {
    case GL_INVALID_ENUM:
      std::cerr << "INVALID_ENUM\n";
      break;
    case GL_INVALID_VALUE:
      std::cerr << "INVALID_VALUE\n";
      break;
    case GL_INVALID_OPERATION:
      std::cerr << "INVALID_OPERATION\n";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      std::cerr << "INVALID_FRAMEBUFFER_OPERATION\n";
      break;
    case GL_OUT_OF_MEMORY:
      std::cerr << "OUT_OF_MEMORY\n";
      break;
    default:
      break;
    }
}

/*
  Code copied from 
  http://www.opengl.org/wiki/Tutorial1:_Rendering_shapes_with_glDrawRangeElements,_VAO,_VBO,_shaders_%28C%2B%2B_/_freeGLUT%29
  and therefore governed by that wiki's terms.
  Code modified by Alan Richardson (arlaari@gmail.com), see earliest git revision of 
  this file for code as it appeared on the wiki when I filched it.
*/
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
std::string loadFile(const char *fname)
{
  std::ifstream file(fname);
  if(!file.is_open())
    {
      std::cout << "Unable to open file " << fname << std::endl;
      exit(1);
    }
  
  std::stringstream fileData;
  fileData << file.rdbuf();
  file.close();
  
  return fileData.str();
}
 
 
// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
void printShaderInfoLog(GLint shader)
{
  int infoLogLen = 0;
  int charsWritten = 0;
  GLchar *infoLog;
  
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
  
  if (infoLogLen > 0)
    {
      infoLog = new GLchar[infoLogLen];
      // error check for fail to allocate memory omitted
      glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
      std::cout << "InfoLog : " << std::endl << infoLog << std::endl;
      delete [] infoLog;
    }
}

constexpr GLuint INVERTEX_ATTRIB_LOC=0, INTEXCOORD0_ATTRIB_LOC=1;
constexpr GLuint INNORMAL_ATTRIB_LOC=2, INCOLOR_ATTRIB_LOC=3;
int LoadShader(const char *pfilePath_vs, const char *pfilePath_fs, bool bindTexCoord0, bool bindNormal, bool bindColor, GLuint &shaderProgram, GLuint &vertexShader, GLuint &fragmentShader)
{
  shaderProgram=0;
  vertexShader=0;
  fragmentShader=0;
  
  // load shaders
  std::string vertexShaderString = loadFile(pfilePath_vs);
  std::string fragmentShaderString = loadFile(pfilePath_fs);
  
  if(vertexShaderString.empty())
    {
      return -1;
    }
  
  if(fragmentShaderString.empty())
    {
      return -1;
    }
  
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	
  
  const char *vertexShaderCStr = vertexShaderString.c_str();
  const char *fragmentShaderCStr = fragmentShaderString.c_str();
  glShaderSource(vertexShader, 1, (const GLchar **)&vertexShaderCStr, nullptr);
  glShaderSource(fragmentShader, 1, (const GLchar **)&fragmentShaderCStr, nullptr);
  
  GLint compiled;
  
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
  if(compiled==GL_FALSE)
    {
      std::cout << "Vertex shader not compiled." << std::endl;
      printShaderInfoLog(vertexShader);
      
      glDeleteShader(vertexShader);
      vertexShader=0;
      glDeleteShader(fragmentShader);
      fragmentShader=0;
      
      return -1;
    }
  
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
  if(compiled==GL_FALSE)
    {
      std::cout << "Fragment shader not compiled." << std::endl;
      printShaderInfoLog(fragmentShader);
      
      glDeleteShader(vertexShader);
      vertexShader=0;
      glDeleteShader(fragmentShader);
      fragmentShader=0;
      
      return -1;
    }
  
  shaderProgram = glCreateProgram();
  
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  
  glBindAttribLocation(shaderProgram, INVERTEX_ATTRIB_LOC, "InVertex");
  
  if(bindTexCoord0)
    glBindAttribLocation(shaderProgram, INTEXCOORD0_ATTRIB_LOC, "InTexCoord0");
  
  if(bindNormal)
    glBindAttribLocation(shaderProgram, INNORMAL_ATTRIB_LOC, "InNormal");
  
  if(bindColor)
    glBindAttribLocation(shaderProgram, INCOLOR_ATTRIB_LOC, "InColor");
  
  glLinkProgram(shaderProgram);
  
  GLint IsLinked;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, (GLint *)&IsLinked);
  if(IsLinked==GL_FALSE)
    {
      std::cout << "Failed to link shader." << std::endl;
      
      GLint maxLength;
      glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
      if(maxLength>0)
	{
	  char *pLinkInfoLog = new char[maxLength];
	  glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, pLinkInfoLog);
	  std::cout << pLinkInfoLog << std::endl;
	  delete [] pLinkInfoLog;
	}
      
      glDetachShader(shaderProgram, vertexShader);
      glDetachShader(shaderProgram, fragmentShader);
      glDeleteShader(vertexShader);
      vertexShader=0;
      glDeleteShader(fragmentShader);
      fragmentShader=0;
      glDeleteProgram(shaderProgram);
      shaderProgram=0;
      
      return -1;
    }
  
  return 1;		//Success
}
#endif // GLUTIL_HPP
