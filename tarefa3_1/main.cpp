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
static GLuint nmap;
static float viewer[4] = {1.0f, 2.0f, 3.0f, 1.0f};
float D = sqrt((viewer[1]*viewer[1])+(viewer[0]*viewer[0])+(viewer[2]*viewer[2]));
float px0;
float py0;
int w = 600;
int h = 600;
static glm::mat4 M(1.0f);

#define INDEX(i,j,nx) ((j)*(nx)+(i))
#define PI	glm::pi<float>()

static void createProgram ()
{
  GLuint vid = CreateShader(GL_VERTEX_SHADER,"vertex.glsl");
  GLuint fid = CreateShader(GL_FRAGMENT_SHADER,"fragment.glsl");
  pid = setProgram(vid,fid,0);
}

static void getTangent (float s, float t, float* x, float* y, float* z)
{
  float theta = s*2*PI;
  float phi = t*PI;
  *x = 2 * PI * cos(theta) * sin(phi);
  *y = 0;
  *z = -2 * PI * sin(theta) * sin(phi);
}

static float* sphereTangents (int nx, int ny)
{
  float* tangents = (float*) malloc(3*(nx+1)*(ny+1) * sizeof(float));
  float dx = 1.0f / nx;
  float dy = 1.0f / ny;
  int n = 0;
  for (int j=0; j<=ny; ++j) {
  	for (int i=0; i<=nx; ++i) {
    	float x, y, z;
      getTangent(i*dx,j*dy,&x,&y,&z);
      tangents[n++] = x;
      tangents[n++] = y;
      tangents[n++] = z;
    }
   }
  return tangents;
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
static float* sphereCoords (int nx, int ny, float** texcoords)
{
  float* coords = (float*) malloc(3*(nx+1)*(ny+1) * sizeof(float));
  (*texcoords) = (float*) malloc(2*(nx+1)*(ny+1) * sizeof(float));
  float dx = 1.0f / nx;
  float dy = 1.0f / ny;
  int n = 0;
  int t = 0;
  for (int j=0; j<=ny; ++j) {
  	for (int i=0; i<=nx; ++i) {
    	float x, y, z;
      getCoord(i*dx,j*dy,&x,&y,&z);
      coords[n++] = x;
      coords[n++] = y;
      coords[n++] = z;
      (*texcoords)[t++] = i*dx;
      (*texcoords)[t++] = j*dy;
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
  int tsize = 2*(nx+1)*(ny+1);
  float* texcoord;
  float* coord = sphereCoords(nx,ny,&texcoord);
	float* tangent = sphereTangents(nx, ny);
  int isize = 6*nx*ny;
  unsigned int* index = sphereIncidence(nx,ny);
  GLuint vao = CreateVAO();

	GLuint bu[3];
	GLuint ind;
  glGenBuffers(3, bu);
	glGenBuffers(1, &ind);
	
	glBindBuffer(GL_ARRAY_BUFFER,bu[0]);
  glBufferData(GL_ARRAY_BUFFER,csize*sizeof(float),(GLvoid*)coord, GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER,bu[1]);
  glBufferData(GL_ARRAY_BUFFER,csize*sizeof(float),(GLvoid*)tangent, GL_STATIC_DRAW);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER,bu[2]);
  glBufferData(GL_ARRAY_BUFFER,tsize*sizeof(float),(GLvoid*)texcoord, GL_STATIC_DRAW);
  glVertexAttribPointer(3,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(3);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ind);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,isize*sizeof(unsigned int),(GLvoid*)index, GL_STATIC_DRAW);
      
	free(index);
  free(coord);
	free(tangent);
	free(texcoord);
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

static void drawSphere(GLuint VAO){
	GLint loc_sampler = glGetUniformLocation(pid, "golfball");
	GLint loc_normal = glGetUniformLocation(pid, "normals");
  glUseProgram(pid);
  glUniform1i(loc_sampler, 0);
	glUniform1i(loc_normal, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, nmap);

	glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 64*64*6, GL_UNSIGNED_INT, 0);
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
 
	tex = CreateTexture("golfball.png");
	nmap = CreateTexture("golfball.png");
  VAO = createSphere(64, 64);
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
	glm::mat4 inv = glm::inverse(mv);
  
	GLint loc_mvp = glGetUniformLocation(pid, "mvp");
	GLint loc_inv = glGetUniformLocation(pid, "inv");

	glUseProgram(pid);
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(loc_inv, 1, GL_FALSE, glm::value_ptr(inv));
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

	glm::vec4 leye = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	GLint loc_leye = glGetUniformLocation(pid, "leye");
	glUseProgram(pid);
  glUniform4fv(loc_leye, 1, glm::value_ptr(leye));

	drawSphere(VAO);

  glutSwapBuffers();
  checkError("Fim da display" );
}

// Reshape callback
static void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

static void motionCB(int x, int y){
	float X0, Y0, Z0, X1, Y1, Z1, l0, l1, r;
	float mod_a, ax, ay, az, theta;
	if(w > h){
		r = h/2;
	}
	else
		r = w/2;

	X0 = (px0-(w/2))/r;
	Y0 = (py0-(h/2))/r;
	l0 = sqrt((X0*X0)+(Y0*Y0));

	X1 = (x-(w/2))/r;
	Y1 = (y-(h/2))/r;
	l1 = sqrt((X1*X1)+(Y1*Y1));

	if(l0 > 1.0){
		X0 = X0/l0;
		Y0 = Y0/l0;
		Z0 = 0.0f;
	}
	else if(l0 <= 1.0){
		Z0 = sqrt(1-(l0*l0));
	}

	if(l1 > 1.0){
		X1 = X1/l1;
		Y1 = Y1/l1;
		Z1 = 0.0f;
	}
	else if(l1 <= 1.0){
		Z1 = sqrt(1-(l1*l1));
	}

	ax = ((Y0*Z1)-(Z0*Y1));
	ay = -((Z0*X1)-(X0*Z1));
	az = ((X0*Y1)-(Y0*X1));
	mod_a = sqrt((ax*ax)+(ay*ay)+(az*az));
	theta = 2*asin(mod_a);
	
	glm::mat4 aux(1.0f);
	aux = glm::translate(aux, glm::vec3(0.0f, 0.0f, -D));
	aux = glm::rotate(aux, theta, glm::vec3(ax, ay, az));
	aux = glm::translate(aux, glm::vec3(0.0f, 0.0f, D));
	M = aux*M;
	
	px0 = x;
	py0 = y;	
	glutPostRedisplay();
}

static void mouseCB(int button, int state, int x, int y){
	if(button != GLUT_LEFT_BUTTON){
		return;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		px0 = float(w-x);
		py0 = float(h-y);
		glutMotionFunc(motionCB);
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		glutMotionFunc(NULL);
	}
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
    glutCreateWindow("Tarefa 3.1");
		glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
		glutMouseFunc(mouseCB);

    // interact... 
    glutMainLoop();
    return 0;
}
