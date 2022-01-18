#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "error.h"
#include "buffer.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

static GLuint vao;
static GLuint pid;
static float viewer[4] = {0.0f, -3.0f, 8.0f, 1.0f};

static void createScene (void)
{
  float coord[] = {
    -0.5f,-0.5f, 0.0f,
     0.5f,-0.5f, 0.0f,
     0.0f, 0.5f, 0.0f,

		0.5f, -1.5f, 0.0f,
		1.5f, -1.0f, 0.0f,
		0.5f,-0.5f, 0.0f,

		-0.5f, -1.5f, 0.0f,
		0.0f, -2.5f, 0.0f,
		0.5f, -1.5f, 0.0f,

		-0.5f, -0.5f, 0.0f,
		-1.5f, -1.0f, 0.0f,
		-0.5f, -1.5f, 0.0f
  };
  float color[] = {
    1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f,
  };
  glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);
  GLuint id[2];
  glGenBuffers(2,id);
  glBindBuffer(GL_ARRAY_BUFFER,id[0]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(coord),coord,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,id[1]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(color),color,GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);
}

static void createProgram ()
{
  GLuint vid = CreateShader(GL_VERTEX_SHADER,"vertex.glsl");
  GLuint fid = CreateShader(GL_FRAGMENT_SHADER,"fragment.glsl");
  pid = setProgram(vid,fid,0);
}

static void initialize()
{
    // open GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    createScene();
    createProgram();
}

static void setCamera(){
	int window[4];
	glGetIntegerv(GL_VIEWPORT, window);
	float ratio = (float) window[2]/window[3];

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 10.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(viewer[0], viewer[1], viewer[2]),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mv = view*model;
	glm::mat4 mvp = proj*mv;

	GLint loc = glGetUniformLocation(pid,"mvp");

	glUseProgram(pid);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));
}

static void display (void)
{
  static int f = 1;
  if (f) {
      initialize();
      f = 0;
  }
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setCamera();

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES,0,12);

  glutSwapBuffers();

  checkError("Fim da display" );
}



// Reshape callback
static void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}


// Keyboard callback
static void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: exit(0); break;
    }
}

// Main function
int main(int argc, char* argv[])
{
    // open GLUT 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowSize(800, 600);

    // create window
    glutCreateWindow("Tarefa 2.1");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // interact... 
    glutMainLoop();
    return 0;
}
