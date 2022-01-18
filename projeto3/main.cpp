/*
Sérgio Gabriel (1611200)
INF1761 - Computação Gráfica
Projeto 3
*/

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

static GLuint pid, sky_pid;
static GLuint vao, sky_vao;
static GLuint nmap, tex_sphere, tex_skybox;
static float viewer[4] = {0.0f, 0.0f, 3.0f, 1.0f};
int w = 600;
int h = 600;
static glm::mat4 M(1.0f);

#define INDEX(i,j,nx) ((j)*(nx)+(i))
#define PI	glm::pi<float>()

static void getTangent (float s, float t, float* x, float* y, float* z){
  float theta = s*2*PI;
  float phi = t*PI;
  *x = 2 * PI * cos(theta) * sin(phi);
  *y = 0;
  *z = -2 * PI * sin(theta) * sin(phi);
}

static float* sphereTangents (int nx, int ny){
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

static void getCoord (float s, float t, float* x, float* y, float* z){
  float theta = s*2*PI;
  float phi = t*PI;
  *x = sin(theta) * sin(phi);
  *y = cos(phi);
  *z = cos(theta) * sin(phi);
}
// allocate and fill the coordinate array of dimension 3*(nx+1)*(ny+1)
static float* sphereCoords (int nx, int ny, float** texcoords){
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
static unsigned int* sphereIncidence (int nx, int ny){
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

static GLuint CreateSphere (int nx, int ny){
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

GLuint CreateTextureSphere(const char* filename){
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

static void createProgram (){
  GLuint vid = CreateShader(GL_VERTEX_SHADER,"vertex.glsl");
  GLuint fid = CreateShader(GL_FRAGMENT_SHADER,"fragment.glsl");
	pid = setProgram(vid,fid,0);

	GLuint vidA = CreateShader(GL_VERTEX_SHADER,"vertexA.glsl");
  GLuint fidA = CreateShader(GL_FRAGMENT_SHADER,"fragmentA.glsl");
	sky_pid = setProgram(vidA,fidA,0);
}

static unsigned char* extract_subimage(int W, int n, unsigned char* data, int x, int y, int w, int h){
	unsigned char* img = (unsigned char*) malloc (w*h*n);

	for(int i=0; i<h; i++){
		memcpy(img+(i*w)*n, data+((y+i)*W+x)*n, w*n);
	}
	return img;
}

GLuint CreateSkyBox(const char* filename){
	int width, height, nchannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nchannels, 0);
	if(!data){
		fprintf(stderr, "Could not load image: %s\n", filename);
		exit(1);
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	//subimages direction
	int w = width/4, h=height/3;
	int x[] = {2*w, 0, w, w, w, 3*w};
	int y[] = {h, h, 0, 2*h, h, h};
	GLenum face[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,	//right
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,	//left
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	//top
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,	//bottom
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,	//front
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 	//back
	};
	for(int i=0; i<6; i++){
		unsigned char* img = extract_subimage(width, nchannels, data, x[i], y[i], w, h);
		glTexImage2D(face[i], 0, GL_RGB, w, h, 0, nchannels==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, img);
		free(img);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_image_free(data);
	return tex;
}

GLuint CreateSkyBoxCube(){
float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	GLuint VAO = CreateVAO();
	GLuint ind;
  glGenBuffers(1, &ind);
	
	glBindBuffer(GL_ARRAY_BUFFER,ind);
  glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices),(GLvoid*)skyboxVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

	return VAO;
}

static void initialize()
{
	// open GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err) {
  	fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    exit(-1);
  }
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
 
	tex_skybox = CreateSkyBox("skybox.jpeg");
	sky_vao = CreateSkyBoxCube();
	tex_sphere = CreateTextureSphere("white.jpg");
	nmap = CreateTextureSphere("golfball.png");
	vao = CreateSphere(64, 64);
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
	GLint loc_mv = glGetUniformLocation(pid, "mv");
	GLint loc_inv = glGetUniformLocation(pid, "inv");
	glUseProgram(pid);
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(loc_inv, 1, GL_FALSE, glm::value_ptr(inv));
	glUniformMatrix4fv(loc_mv, 1, GL_FALSE, glm::value_ptr(mv));
	

	GLint loc_sky = glGetUniformLocation(sky_pid, "sky");
	GLint loc_mvp2 = glGetUniformLocation(sky_pid, "mvp");
  glUseProgram(sky_pid);
  glUniform1i(loc_sky, 0);
	glUniformMatrix4fv(loc_mvp2, 1, GL_FALSE, glm::value_ptr(mvp));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_skybox);
	glDepthMask(GL_FALSE);
	glBindVertexArray(sky_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

static void drawSphere(GLuint VAO){
	GLint loc_sampler = glGetUniformLocation(pid, "golfball");
	GLint loc_normal = glGetUniformLocation(pid, "normals");
	GLint loc_sky = glGetUniformLocation(pid, "sky");
	glUseProgram(pid);
  glUniform1i(loc_sampler, 0);
	glUniform1i(loc_normal, 1);
	glUniform1i(loc_sky, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_sphere);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, nmap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_skybox);

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

	glm::vec4 leye = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	GLint loc_leye = glGetUniformLocation(pid, "leye");
	glUseProgram(pid);
  glUniform4fv(loc_leye, 1, glm::value_ptr(leye));

	drawSphere(vao);

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
    glutCreateWindow("Projeto 3");
		glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // interact... 
    glutMainLoop();
    return 0;
}
