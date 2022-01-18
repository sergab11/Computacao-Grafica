#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "error.h"
#include "buffer.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

static GLuint VAO;
static GLuint pid;
static GLuint tex;
static float viewer[4] = {1.0f, 2.0f, 3.0f, 1.0f};
int w = 600;
int h = 600;
static glm::mat4 M(1.0f);

static void createProgram (){
  GLuint vid = CreateShader(GL_VERTEX_SHADER, "vertex.glsl");
	GLuint cid = CreateShader(GL_TESS_CONTROL_SHADER, "control.glsl");
	GLuint eid = CreateShader(GL_TESS_EVALUATION_SHADER, "evaluation.glsl");
  GLuint fid = CreateShader(GL_FRAGMENT_SHADER, "fragment.glsl");
  pid = setProgram(vid, cid, eid, fid, 0);
}

static GLuint CreatePatch(void){
	float geom[] = {
	0.0f, 0.0f, 0.0f, 2.0f
	};
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	GLuint vao = CreateVAO();

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(geom), (GLvoid*)geom, GL_STATIC_DRAW);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

	return vao;
}

GLuint CreateTexture(const char* filename){
	GLuint tex;
	int width, height, nchannels;
	unsigned char *data = stbi_load(filename, &width, &height, &nchannels, 3);
	if(data == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return tex;
}

static void initialize(){
	// open GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err) {
  	fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    exit(-1);
  }
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
 
	tex = CreateTexture("golfball.png");	
	VAO = CreatePatch();		
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
	glm::mat4 mv = M*view*model;
	glm::mat4 mvp = proj*mv; 
	glm::mat4 nm = glm::transpose(glm::inverse(mv));
  
	GLint loc_mv = glGetUniformLocation(pid, "mv");	
	GLint loc_mvp = glGetUniformLocation(pid, "mvp");
	GLint loc_nm = glGetUniformLocation(pid, "nm");
	glUseProgram(pid);
	glUniformMatrix4fv(loc_mv, 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(loc_nm, 1, GL_FALSE, glm::value_ptr(nm));
}

static void drawTess(GLuint vao){
	GLint loc_sampler = glGetUniformLocation(pid, "golfball");
	glUseProgram(pid);	
	glUniform1i(loc_sampler, 0);		

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBindVertexArray(vao);
	glDrawArrays(GL_PATCHES, 0, 1);
}

static void display (void){
	static int f = 1;
  if (f) {
      initialize();
      f = 0;
  }
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();
	drawTess(VAO);

  glutSwapBuffers();
  checkError("Fim da display" );
}

// Reshape callback
static void reshape(int w, int h){
	glViewport(0, 0, w, h);
}

// Keyboard callback
static void keyboard(unsigned char key, int x, int y){
  switch (key){
  	case 27: exit(0); break;
  }
}

// Main function
int main(int argc, char* argv[]){
  // open GLUT 
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(w, h);

  // create window
  glutCreateWindow("Tarefa 3.2");
	glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);

  // interact... 
  glutMainLoop();
  return 0;
}
