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

#include "error.h"
#include "buffer.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#define INDEX(i,j,nx) ((j)*(nx)+(i))
#define PI	glm::pi<float>()

struct Material {
	glm::vec4 amb;
	glm::vec4 dif;
	glm::vec4 spec;
	float shi;
};

static Material vermelho = {
	glm::vec4(0.41f, 0.13f, 0.21f, 1.0f),
	glm::vec4(0.91f, 0.0f, 0.0f, 1.0f),
	glm::vec4(0.5f, 0.3f, 0.1f, 1.0f),
	64
};

static Material marrom = {
	glm::vec4(0.21f, 0.13f, 0.05f, 1.0f),
	glm::vec4(0.91f, 0.43f, 0.18f, 1.0f),
	glm::vec4(0.39f, 0.27f, 0.17f, 1.0f),
	64
};

static Material azul = {
	glm::vec4(0.05f, 0.13f, 0.21f, 1.0f),
	glm::vec4(0.3f, 0.3f, 1.0f, 1.0f),
	glm::vec4(0.39f, 0.27f, 0.17f, 1.0f),
	64
};

static Material branco = {
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	64
};

static Material verde = {
	glm::vec4(0.1f, 0.53f, 0.21f, 1.0f),
	glm::vec4(0.0f, 0.91f, 0.0f, 1.0f),
	glm::vec4(0.3f, 0.5f, 0.1f, 1.0f),
	64
};

struct Lights {
	glm::vec4 pos;
	glm::vec3 dir;
	float abert;
};

static Lights spot = {
	glm::vec4(0.0f, 5.0f, 0.0f, 1.0f),
	glm::vec3(0.f, -1.0f, 0.0f),
	glm::radians(75.0f)
};

static GLuint vao_cube[9];
static GLuint vao_sphere;
static GLuint vao_paper;
static GLuint pid;
static float viewer[4] = {3.0f, 3.0f, 1.0f, 1.0f};
int w = 600;
int h = 600;
static glm::mat4 M(1.0f);

static GLuint createPaper(){
	float normal[] = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
	};
	float coord[] = {
	-0.5f, 0.0f, 1.0f,
	0.5f, 0.0f, 1.0f,
	0.5f, 0.0f, -1.0f,
	-0.5f, 0.0f, -1.0f
	};

	GLuint index[] = {0, 1, 2, 0, 2, 3};

	GLuint vao = CreateVAO();

	GLuint cn[2];
	GLuint ind;
  glGenBuffers(2, cn);
	glGenBuffers(1, &ind);
	
	glBindBuffer(GL_ARRAY_BUFFER,cn[0]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(coord),(GLvoid*)coord, GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER,cn[1]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(normal),(GLvoid*)normal, GL_STATIC_DRAW);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ind);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),(GLvoid*)index, GL_STATIC_DRAW);

	return vao;
}

static GLuint createCube ()
{
	float coord[] = {
  -0.5, -0.5, -0.5,
	-0.5, 0.5, -0.5,
	0.5, 0.5, -0.5,
	0.5, -0.5, -0.5,

	-0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	-0.5, 0.5, -0.5,
	-0.5, -0.5, -0.5,

	-0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, -0.5, 0.5,

	0.5, -0.5, 0.5,
	0.5, 0.5, 0.5,
	0.5, 0.5, -0.5,
	0.5, -0.5, -0.5,

	-0.5, 0.5, 0.5,
	-0.5, 0.5, -0.5,
	0.5, 0.5, -0.5,
	0.5, 0.5, 0.5,

	-0.5, -0.5, 0.5,
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, -0.5, 0.5
  };

	float normal [] = {
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,

	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,

	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,

	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,

	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,

	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0	
	};

  GLuint index [] = {
	0, 1, 2, 0, 2, 3, 
	4, 5, 6, 4, 6, 7, 
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
	};

	GLuint vao = CreateVAO();

  GLuint id[2];
	GLuint ind;
  glGenBuffers(2,id);
	glGenBuffers(1, &ind);

  glBindBuffer(GL_ARRAY_BUFFER,id[0]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(coord),(void*)coord,GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER,id[1]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(normal),(void*)normal,GL_STATIC_DRAW);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ind);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(index),(void*)index,GL_STATIC_DRAW);

	return vao;
}

static void getCoord (float s, float t, float* x, float* y, float* z)
{
  float theta = s*2*PI;
    float phi = t*PI;
    *x = sin(theta) * sin(phi);
    *y = cos(phi);
  	*z = cos(theta) * sin(phi);
}
// allocate and fill the coordinate array of dimension 3*(nx+1)*(ny+1)
static float* sphereCoords (int nx, int ny)
{
  float* coords = (float*) malloc(3*(nx+1)*(ny+1) * sizeof(float));
  float dx = 1.0f / nx;
  float dy = 1.0f / ny;
  int n = 0;
  for (int j=0; j<=ny; ++j) {
  	for (int i=0; i<=nx; ++i) {
    	float x, y, z;
      getCoord(i*dx,j*dy,&x,&y,&z);
      coords[n++] = x;
      coords[n++] = y;
      coords[n++] = z;
     }
   }
  return coords;
}

// allocate and fill the incidence array of dimension 6*nx*ny
static unsigned int* sphereIncidence (int nx, int ny)
{
  unsigned int* inc = (unsigned int*) malloc((6*nx*ny) * sizeof(unsigned int));
  int n = 0;
  for (int j=0; j<ny; ++j) {
  	for (int i=0; i<nx; ++i) {
    	inc[n++] = INDEX(i,j,nx);
      inc[n++] = INDEX(i+1,j+1,nx);
      inc[n++] = INDEX(i+1,j,nx);
      inc[n++] = INDEX(i,j,nx);
      inc[n++] = INDEX(i,j+1,nx);
      inc[n++] = INDEX(i+1,j+1,nx);
     }
   }
   return inc;
}

static GLuint createSphere (int nx, int ny)
{
	int csize = 3*(nx+1)*(ny+1);
  float* coord = sphereCoords(nx,ny);
  int isize = 6*nx*ny;
  unsigned int* index = sphereIncidence(nx,ny);
  GLuint vao = CreateVAO();

	GLuint cn;
	GLuint ind;
  glGenBuffers(1, &cn);
	glGenBuffers(1, &ind);
	
	glBindBuffer(GL_ARRAY_BUFFER,cn);
  glBufferData(GL_ARRAY_BUFFER,csize*sizeof(float),(GLvoid*)coord, GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ind);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,isize*sizeof(unsigned int),(GLvoid*)index, GL_STATIC_DRAW);
      
	free(index);
  free(coord);
  return vao;
}

static void createProgram ()
{
  GLuint vid = CreateShader(GL_VERTEX_SHADER,"vertex.glsl");
  GLuint fid = CreateShader(GL_FRAGMENT_SHADER,"fragment.glsl");
  pid = setProgram(vid,fid,0);
}

static void iniMaterials(){
	GLuint ubuffer[5];
	GLuint index = glGetUniformBlockIndex(pid, "Material");
	glGenBuffers(5, ubuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, ubuffer[0]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), (GLvoid*) &vermelho, GL_STATIC_DRAW);
	glUniformBlockBinding(pid, index, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubuffer[0]);

	glBindBuffer(GL_UNIFORM_BUFFER, ubuffer[1]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), (GLvoid*) &marrom, GL_STATIC_DRAW);
	glUniformBlockBinding(pid, index, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubuffer[1]);

	glBindBuffer(GL_UNIFORM_BUFFER, ubuffer[2]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), (GLvoid*) &azul, GL_STATIC_DRAW);
	glUniformBlockBinding(pid, index, 2);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubuffer[2]);

	glBindBuffer(GL_UNIFORM_BUFFER, ubuffer[3]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), (GLvoid*) &branco, GL_STATIC_DRAW);
	glUniformBlockBinding(pid, index, 3);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubuffer[3]);

	glBindBuffer(GL_UNIFORM_BUFFER, ubuffer[4]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), (GLvoid*) &verde, GL_STATIC_DRAW);
	glUniformBlockBinding(pid, index, 4);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, ubuffer[4]);
}

static void iniLights(){
	GLuint ubuffer;
	GLuint index = glGetUniformBlockIndex(pid, "Lights");
	glGenBuffers(1, &ubuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, ubuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), (GLvoid*) &spot, GL_STATIC_DRAW);
	glUniformBlockBinding(pid, index, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubuffer);
}

static void initialize()
{
		// open GLEW
    GLenum err = glewInit();
		if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
 
    for(int i=0; i<9; i++)
			vao_cube[i] = createCube();
		vao_sphere = createSphere(64, 64);
		vao_paper = createPaper();
    createProgram();
		iniMaterials();
		iniLights();			
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

static void drawPaper(GLuint VAO, float sx, float sy, float sz, float tx, float ty, float tz){
	glm::vec4 sc = glm::vec4(sx, sy, sz, 1.0f);
	glm::vec4 trans = glm::vec4(tx, ty, tz, 1.0f);
	GLint loc_scale = glGetUniformLocation(pid, "scale");
	GLint loc_translate = glGetUniformLocation(pid, "translate");
  glUseProgram(pid);
  glUniform4fv(loc_scale, 1, glm::value_ptr(sc));
	glUniform4fv(loc_translate, 1, glm::value_ptr(trans));

	glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void drawCube(GLuint VAO, float sx, float sy, float sz, float tx, float ty, float tz){
	glm::vec4 sc = glm::vec4(sx, sy, sz, 1.0f);
	glm::vec4 trans = glm::vec4(tx, ty, tz, 1.0f);
	GLint loc_scale = glGetUniformLocation(pid, "scale");
	GLint loc_translate = glGetUniformLocation(pid, "translate");
  glUseProgram(pid);
  glUniform4fv(loc_scale, 1, glm::value_ptr(sc));
	glUniform4fv(loc_translate, 1, glm::value_ptr(trans));

	glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

static void drawSphere(GLuint VAO, float sx, float sy, float sz, float tx, float ty, float tz){
	glm::vec4 sc = glm::vec4(sx, sy, sz, 1.0f);
	glm::vec4 trans = glm::vec4(tx, ty, tz, 1.0f);
	GLint loc_scale = glGetUniformLocation(pid, "scale");
	GLint loc_translate = glGetUniformLocation(pid, "translate");
  glUseProgram(pid);
  glUniform4fv(loc_scale, 1, glm::value_ptr(sc));
	glUniform4fv(loc_translate, 1, glm::value_ptr(trans));

	glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 64*64*6, GL_UNSIGNED_INT, 0);
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
	glm::vec4 aleye = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	GLint loc_aleye = glGetUniformLocation(pid, "aleye");
	glUseProgram(pid);
  glUniform4fv(loc_aleye, 1, glm::value_ptr(aleye));
	
	int index = glGetUniformBlockIndex(pid, "Material");
	//desenha tampo da mesa marrom
	glUniformBlockBinding(pid, index, 1);
	drawCube(vao_cube[0], 2.0f, 0.2f, 2.0f, 0.0, -0.5, 0.0);
	//desenha os 4 "pés" da mesa	
	drawCube(vao_cube[1], 0.2f, 2.0f, 0.2f, 4.0, -0.5, 4.0);
	drawCube(vao_cube[2], 0.2f, 2.0f, 0.2f, 4.0, -0.5, -4.0);
	drawCube(vao_cube[3], 0.2f, 2.0f, 0.2f, -4.0, -0.5, -4.0);
	drawCube(vao_cube[4], 0.2f, 2.0f, 0.2f, -4.0, -0.5, 4.0);
	//desenha 4 cubos azuis em cima da mesa
	glUniformBlockBinding(pid, index, 2);
	drawCube(vao_cube[5], 0.25f, 0.25f, 0.25f, 2.0, 0.5, 0.0);
	drawCube(vao_cube[6], 0.25f, 0.25f, 0.25f, 0.0, 0.5, -2.0);
	drawCube(vao_cube[7], 0.25f, 0.25f, 0.25f, -2.0, 0.5, 0.0);
	drawCube(vao_cube[8], 0.25f, 0.25f, 0.25f, 0.0, 0.5, 2.0);
	//desenha 1 esfera verde em cima da mesa
	glUniformBlockBinding(pid, index, 4);
	drawSphere(vao_sphere, 0.2f, 0.2f, 0.2f, 0.0f, 0.75f, 0.0f);
	//desenha 1 folha de papel em cima da mesa
	glUniformBlockBinding(pid, index, 3);
	drawPaper(vao_paper, 0.25f, 0.25f, 0.25f, 3.0f, 0.05f, 3.0f);

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
    glutInitWindowSize(w, h);

    // create window
    glutCreateWindow("Projeto2");
		glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // interact... 
    glutMainLoop();
    return 0;
}
