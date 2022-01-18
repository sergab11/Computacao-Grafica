#include "buffer.h"
#include "error.h"

#include <stdio.h>

GLuint CreateVAO (void)
{
  GLuint vao;
  glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);
  return vao;
}

void DeleteVAO (GLuint vao)
{
  glDeleteBuffers(1,&vao);
}

GLuint CreateBuffer (GLenum target, GLsizeiptr size, const GLvoid* data)
{
  GLuint id;
  glGenBuffers(1,&id);
  glBindBuffer(target,id);
  glBufferData(target,size,data,GL_STATIC_DRAW);
  return id;
}

void DeleteBuffer (GLuint id)
{
  glDeleteBuffers(1,&id);
}

void VertexAttribPointer (GLuint index, GLint size, GLenum type, 
                          GLboolean norm, GLsizei stride, 
                          const GLvoid* offset)
{
  glVertexAttribPointer(index,size,type,norm,stride,offset);
  glEnableVertexAttribArray(index);
}

