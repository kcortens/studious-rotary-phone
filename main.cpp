#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

const float trianglePos[] = {
	0.0f, 0.8f, 0.0f, 1.0f,
	0.8f, -0.5856f, 0.0f, 1.0f,
	-0.8f, -0.5856f, 0.0f, 1.0f
};

GLuint elapsedTimeUni;
GLuint testObj;
GLuint shaderProg;

static void assignGlutCB();
static void renderCB();
static void initShaders();
static void initTestObj();
static void reshapeCB(int w, int h);
static void keyboardCB(unsigned char key, int x, int y);
GLuint createShader(GLenum shaderType, const std::string &shaderFilename);
GLuint createShaderProg(const std::vector<GLuint> &shaderList);

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(50,50);
	glutCreateWindow("ooooo baby");

	assignGlutCB();

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		fprintf(stderr, "glew init error: %s\n", glewGetErrorString(err));
		return 1;
	}

	initShaders();
	initTestObj();

	glutMainLoop();
	return 0;
}

GLuint createShader(GLenum shaderType, const std::string &shaderFilename) {
	std::string fileContents = "";
	std::string curLine;
	std::ifstream shaderFile;//(shaderFilename);
	shaderFile.open(shaderFilename.c_str(), std::ifstream::in);

	if(shaderFile.is_open()) {
		while(getline(shaderFile,curLine)) fileContents.append(curLine + '\n');
		shaderFile.close();
	}

	GLuint shader = glCreateShader(shaderType);
	const char* fileContentsCStr = fileContents.c_str();
	glShaderSource(shader, 1, &fileContentsCStr, NULL);

	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "shader fucked up: %s\n", strInfoLog);
	}

	return shader;
}

GLuint createShaderProg(const std::vector<GLuint> &shaderList) {
	GLuint prog = glCreateProgram();

	for(size_t i=0;i<shaderList.size();i++) glAttachShader(prog,shaderList[i]);
	glLinkProgram(prog);

	GLint linkStatus;
	glGetProgramiv(prog, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE) fprintf(stderr, "error linking program");

	for(size_t i=0;i<shaderList.size();i++) {
		glDetachShader(prog,shaderList[i]);
		glDeleteShader(shaderList[i]);
	}

	return prog;
}

static void assignGlutCB() {
	glutDisplayFunc(renderCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
}

static void initShaders() {
	std::vector<GLuint> shaderVector;

	shaderVector.push_back(createShader(GL_VERTEX_SHADER, "./vshader.glsl"));
	shaderVector.push_back(createShader(GL_FRAGMENT_SHADER, "./fshader.glsl"));
	shaderProg = createShaderProg(shaderVector);

	elapsedTimeUni = glGetUniformLocation(shaderProg, "time");
	GLuint loopDurUni = glGetUniformLocation(shaderProg, "loopDur");
	glUseProgram(shaderProg);
	glUniform1f(loopDurUni, 5.0f);
	glUseProgram(0);
}

static void initTestObj() {
	glGenBuffers(1, &testObj);

	glBindBuffer(GL_ARRAY_BUFFER, testObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trianglePos), trianglePos, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void renderCB() {
	glClearColor( 255.0f, 255.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProg);
	glUniform1f(elapsedTimeUni, glutGet(GLUT_ELAPSED_TIME) / 1000.0f);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, testObj);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLES,0,3);
	glDisableVertexAttribArray(0);

	glUseProgram(0);

	glutSwapBuffers();
	glutPostRedisplay();
}

static void reshapeCB(int w, int h) {
	glViewport(0,0,(GLsizei)w, (GLsizei)h);
}

static void keyboardCB(unsigned char key, int x, int y) {
	switch(key) {
		case 27:
			glutLeaveMainLoop();
			return;
	}
}
