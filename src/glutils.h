#ifndef _MY_GL_UTILS_
#define _MY_GL_UTILS_

typedef struct
{
	char* src;	// Null-terminated string with the shader source code
	int	size; // The size of the string
} mgl_shaderSource;

int mgl_compileProgram(char* vertexSource, char* fragmentSource);

#endif//_MY_GL_UTILS_
