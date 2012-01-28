/* GLMatrix.h
 * Include LilyEngineUtils' matrix template, and implement some convenience functions
 * using GLfloat instead of float.
 * Some convenience functions are trivially modified versions of functions appearing
 * in LilyEngineUtils. They appear after the large copyright notice.
 */

#include "libLilyEngineUtils/matrix.h"

typedef ExPop::Matrix<GLfloat,4,4> GLFMatrix4x4;
typedef ExPop::Matrix<GLfloat,4,1> glVec;

/// Make an OpenGL-compatible orthographic projection matrix

inline GLFMatrix4x4 makeOrtho(GLfloat left, GLfloat right, 
			      GLfloat top, GLfloat bottom,
			      GLfloat near, GLfloat far)
{
  GLFMatrix4x4 mat;
  const GLfloat width=right-left,
    height=top-bottom,
    depth=far-near;

  mat(0,0) = 2.0f/width;
  mat(1,1) = 2.0f/height;
  mat(2,2) = 2.0f/depth;
  mat(3,3) = 1.0f;
  mat(0,3) =-(right+left)/width;
  mat(1,3) =-(top+bottom)/height;
  mat(2,3) = -(far+near)/depth;
  return mat;
}

// Duplicate copyright notice in case this file gets distributed apart from
// the Lily header

// ---------------------------------------------------------------------------
//
// Copyright (c) 2011 Clifford Jolly
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product
// documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
// distribution.
//
// -------------------------- END HEADER -------------------------------------


/// Make a 4x4 GLfloat translation matrix.
inline GLFMatrix4x4 makeTranslationMatrix(const glVec &t) {
  
  GLFMatrix4x4 mat;
  mat(0, 3) = t.x;
  mat(1, 3) = t.y;
  mat(2, 3) = t.z;
  return mat;
}

/// Make a 4x4 GLfloat rotation matrix.
inline GLFMatrix4x4 makeRotationMatrix(const glVec &axis, GLfloat angle) {

  float c = std::cos(angle);
  float s = std::sin(angle);
  float t = 1 - c;
  
  GLFMatrix4x4 mat;
  
  mat(0, 0) = t*axis.x*axis.x + c;
  mat(0, 1) = t*axis.x*axis.y + s*axis.z;
  mat(0, 2) = t*axis.x*axis.z - s*axis.y;
  
  mat(1, 0) = t*axis.y*axis.x - s*axis.z;
  mat(1, 1) = t*axis.y*axis.y + c;
  mat(1, 2) = t*axis.y*axis.z + s*axis.x;
  
  mat(2, 0) = t*axis.z*axis.x + s*axis.y;
  mat(2, 1) = t*axis.z*axis.y - s*axis.x;
  mat(2, 2) = t*axis.z*axis.z + c;
  
  return mat;
}

/// Make a 4x4 GLfloat scaling matrix.
inline GLFMatrix4x4 makeScaleMatrix(const glVec &scale) {
  
  GLFMatrix4x4 mat;
  
  mat(0, 0) = scale.x;
  mat(1, 1) = scale.y;
  mat(2, 2) = scale.z;
  
  return mat;
}

