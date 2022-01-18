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

static GLuint pid;
static GLuint vao;
static GLuint skyb;
static float viewer[4] = {2.0f, 2.0f, 5.0f, 1.0f};
int w = 600;
int h = 600;
static glm::mat4 M(1.0f);

static void createProgram ()
{
  GLuint vid = CreateShader(GL_VERTEX_SHADER,"vertex.glsl");
  GLuint fid = CreateShader(GL_FRAGMENT_SHADER,"fragment.glsl");
  pid = setProgram(vid,fid,0);
}

static GLuint createCubeSkyBox(){
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
	GLuint skyboxVAO = CreateVAO();
  unsigned int skyboxVBO;
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), (GLvoid*)skyboxVertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	return skyboxVAO;
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
 
	skyb = CreateSkyBox("skybox.jpeg");
	vao = createCubeSkyBox();
	createProgram();
}

static void setCamera(){
	int window[4];
	glGetIntegerv(GL_VIEWPORT, window);
	float ratio = (float) window[2]/window[3];

	GLint loc_sampler = glGetUniformLocation(pid, "sky");
	glUseProgram(pid);
  glUniform1i(loc_sampler, 0);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 10.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(viewer[0], viewer[1], viewer[2]),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mv = M*view*model;
	glm::mat4 mvp = proj*mv;
  
	GLint loc_mvp = glGetUniformLocation(pid, "mvp");
	glUseProgram(pid);
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyb);

	glDepthMask(GL_FALSE);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
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
    glutCreateWindow("Tarefa 3.3");
		glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // interact... 
    glutMainLoop();
    return 0;
}
