#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static char* Concatenate (char* str, const char* append)
{
  if (str) {
    char* p = (char*) realloc(str,strlen(str) + strlen(append) + 1);
    if (p) str = p;
  }
  else {
    str = (char*) malloc(strlen(append) + 1);
    if (str) *str = 0;
  }
  if (!str) {
    fprintf(stderr,"Not enough memory to store shader source\n");
    exit(1);
  }
  strcat(str,append);
  return str;
}

static GLchar* ReadFile (const char* filename)
{
  char* code = 0;
  FILE* fp = fopen(filename,"r");
  if (!fp) {
    fprintf(stderr,"Could not open shader file: %s\n",filename);
    exit(1);
  }
  char line[BUFSIZ];
  while (fgets(line,BUFSIZ,fp)) 
    code = Concatenate(code,line);
  fclose(fp);
  return code;
}

static void CompileShader (GLuint id)
{
  GLint status;
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);
  if (!status) {
     GLint len;
     glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
     char* message = (char*) malloc((len+1)*sizeof(char));
     glGetShaderInfoLog(id, len, 0, message);
     fprintf(stderr,"%s\n",message);
     free(message);
     exit(1);
   }
}

static void LinkProgram (GLuint id)
{
  GLint status;
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &status);
  if (!status) {
    GLint len;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
    char* message = (char*) malloc((len+1)*sizeof(char));
    glGetProgramInfoLog(id, len, 0, message);
    fprintf(stderr,"%s\n",message);
    free(message);
    exit(1);
  }
}

GLuint CreateShader (GLenum shadertype, const char* filename)
{
  GLuint id = glCreateShader(shadertype);
  if (id==0) {
    fprintf(stderr,"Could not create shader object");
    exit(1);
  }
  char* source = ReadFile(filename);
  glShaderSource(id, 1, &source, 0);
  CompileShader(id);
  free(source);
  return id;
}

// Create program. The end of the list of shader id must be indicated by zero
GLuint setProgram (int sid, ...)
{
  GLuint pid = glCreateProgram();
  if (pid==0) {
    fprintf(stderr,"Could not create program object");
    exit(1);
  }
  va_list ap;
  va_start(ap,sid);
  while (sid != 0) {
    glAttachShader(pid,sid);
    sid = va_arg(ap,int);
  }
  va_end(ap);
  LinkProgram(pid);
  return pid;
}
