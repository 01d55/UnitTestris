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
			      GLfloat bottom, GLfloat top,
			      GLfloat near, GLfloat far)
{
  return ExPop::makeOrthoMatrix(left,right,bottom,top,near,far);
}


/// Make a 4x4 GLfloat translation matrix.
inline GLFMatrix4x4 makeTranslationMatrix(const glVec &t) {
  ExPop::MatSize<GLfloat>::Vec3 vec(t.x,t.y,t.z);
  return ExPop::makeTranslationMatrix<GLfloat>(vec);
}

/// Make a 4x4 GLfloat rotation matrix.
inline GLFMatrix4x4 makeRotationMatrix(const glVec &axis, GLfloat angle) {
  ExPop::MatSize<GLfloat>::Vec3 vec(axis.x,axis.y,axis.z);
  return ExPop::makeRotationMatrix(vec,angle);
}

/// Make a 4x4 GLfloat scaling matrix.
inline GLFMatrix4x4 makeScaleMatrix(const glVec &scale) {
  ExPop::MatSize<GLfloat>::Vec3 vec(scale.x,scale.y,scale.z);
  return ExPop::makeScaleMatrix<GLfloat>(vec);
}

