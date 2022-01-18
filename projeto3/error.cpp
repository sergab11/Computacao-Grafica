#include <GL/glew.h>
#include <GL/gl.h>

#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void checkError (const char* msg)
{
  GLenum err = glGetError();
  if (err == GL_NO_ERROR) 
    return;
  switch(err) {
    case GL_INVALID_ENUM: fprintf(stderr, "GL error: GL_INVALID_ENUM (%s)\n",msg?msg:""); break;
    case GL_INVALID_VALUE: fprintf(stderr, "GL error: GL_INVALID_VALUE (%s)\n",msg?msg:""); break;
    case GL_INVALID_OPERATION: fprintf(stderr, "GL error: GL_INVALID_OPERATION (%s)\n",msg?msg:""); break;
    case GL_OUT_OF_MEMORY: fprintf(stderr, "GL error: GL_OUT_OF_MEMORY (%s)\n",msg?msg:""); break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: fprintf(stderr, "GL error: GL_INVALID_FRAMEBUFFER_OPERATION (%s)\n",msg?msg:""); break;
    default: fprintf(stderr, "GL error: unknown error (%s)\n",msg?msg:""); break;
  }
  exit(1);
}
