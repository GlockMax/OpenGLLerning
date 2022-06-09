#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <string>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Input handler
void handleKeys(unsigned char key, int x, int y);

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//Shader loading utility programs
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Graphics program
GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLuint gVBO[2];
GLuint gVAO = 0;

bool init() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}
	//Use OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}

	//Create context
	gContext = SDL_GL_CreateContext(gWindow);
	if (gContext == NULL) {
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
	}

	//Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
	}

	//Initialize OpenGL
	if (!initGL()) {
		std::cout << "Unable to initialize OpenGL!" << std::endl;
		return false;
	}
	
	return true;
}

bool initGL() {
	//Generate program
	gProgramID = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//TODO: ַהוסü המכזום בûעü פאיכ
	const GLchar* vertexShaderSource[] =
	{
		"#version 430\nlayout (location=0) in vec3 VertexPosition; layout (location=1) in vec3 VertexColor; out vec3 Color; void main() { Color = VertexColor; gl_Position = vec4(VertexPosition, 1.0); }"
	};

	//Set vertex source
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

	//Compile vertex source
	glCompileShader(vertexShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE) {
		std::cout << "Unable to compile vertex shader " << vertexShader << std::endl;
		printShaderLog(vertexShader);
		return false;
	}
	//Attach vertex shader to program
	glAttachShader(gProgramID, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//TODO: ַהוסü עמזו המכזום בûעü פאיכ
	const GLchar* fragmentShaderSource[] =
	{
		"#version 430\nin vec3 Color; out vec4 FragColor; void main() { FragColor = vec4(Color, 1.0); }"
	};

	//Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE) {
		std::cout << "Unable to compile fragment shader " << fragmentShader << std::endl;
		printShaderLog(fragmentShader);
		return false;
	}
	//Attach fragment shader to program
	glAttachShader(gProgramID, fragmentShader);

	//Link program
	glLinkProgram(gProgramID);

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		std::cout << "Error linking program " << gProgramID << std::endl;
		printProgramLog(gProgramID);
		return false;
	}
	//Get vertex attribute location
	//gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
	//if (gVertexPos2DLocation == -1) {
		//std::cout << "LVertexPos2D is not a valid glsl program variable!" << std::endl;
		//return false;
	std::cout << glGetAttribLocation(gProgramID, "VertexColor") << std::endl;
	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//VBO data
	GLfloat vertexData[] = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.8f, 0.0f
	};

	GLfloat color_data[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};


	//Create VBO
	//GLuint gVBO[2];
	glGenBuffers(2, gVBO);
	GLuint pb = gVBO[0];
	GLuint pc = gVBO[1];
	glBindBuffer(GL_ARRAY_BUFFER, pb);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, pc);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), color_data, GL_STATIC_DRAW);

	//Create VAO
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, pb);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, pc);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	return true;
}

void handleKeys(unsigned char key, int x, int y) {
	//Toggle quad
	gRenderQuad = key == 'q' ? !gRenderQuad : gRenderQuad;
}

void update() {
	//No per frame update needed
}

void render() {
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Render quad
	if (gRenderQuad) {
		//Bind program
		glUseProgram(gProgramID);

		glBindVertexArray(gVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Unbind program
		glUseProgram(NULL);
	}
}

void close() {
	//Deallocate program
	glDeleteProgram(gProgramID);

	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void printProgramLog(GLuint program) {
	//Make sure name is shader
	if (glIsProgram(program)) {
		//Program log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[maxLength];

		//Get info log
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) std::cout << infoLog << std::endl;

		//Deallocate string
		delete[] infoLog;
	}
	else {
		std::cout << "Name " << program << " is not a program!" << std::endl;
	}
}

void printShaderLog(GLuint shader) {
	//Make sure name is shader
	if (glIsShader(shader)) {
		//Shader log length
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		//Get info string length
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//Allocate string
		char* infoLog = new char[maxLength];

		//Get info log
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0) std::cout << infoLog << std::endl;

		//Deallocate string
		delete[] infoLog;
	}
	else {
		std::cout << "Name " << shader << " is not a shader!" << std::endl;
	}
}

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!init()) {
		std::cout << "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Enable text input
	SDL_StartTextInput();

	//While application is running
	while (!quit) {
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			//Handle keypress with current mouse position
			else if (e.type == SDL_TEXTINPUT)
			{
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				handleKeys(e.text.text[0], x, y);
			}
		}

		//Render quad
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	//Disable text input
	SDL_StopTextInput();

	//Free resources and close SDL
	close();

	return 0;
}