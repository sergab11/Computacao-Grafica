#define BUFFER_H

#include <GL/glew.h>
#include <GL/gl.h>


GLuint CreateVAO (void);
void DeleteVAO (GLuint vao);

GLuint CreateBuffer (GLenum target, GLint size, const GLvoid* data);
void VertexAttribPointer (GLuint index, GLint size, GLenum type, 
                          GLboolean norm, GLsizei stride, 
                          const GLvoid* offset);
void DeleteBuffer (GLuint id);


