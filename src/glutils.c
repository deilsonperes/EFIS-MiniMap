#include "glutils.h"

#include <stdio.h>
#include <errno.h>
#include "GL/glew.h"
#include "GL/GL.h"

// Function declaration
int mgl_compileShader(GLenum, char*);
mgl_shaderSource* mgl_loadShaderFile(char*);
void mgl_shaderSourceDelete(mgl_shaderSource*);

int mgl_compileProgram(char* vertexSource, char* fragmentSource)
{
	// Load shaders
	GLuint glVertex = mgl_compileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint glFragment = mgl_compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	// Create and link shader program
	GLuint glProgram = glCreateProgram();
	glAttachShader(glProgram, glVertex);
	glAttachShader(glProgram, glFragment);
	glLinkProgram(glProgram);

	// Check for errors
	GLint glCompileStatus;
	glGetProgramiv(glProgram, GL_COMPILE_STATUS, &glCompileStatus);
	if(glCompileStatus == GL_FALSE)
	{
		GLint glLogLength = 0;
		GLchar *glLog = malloc(sizeof(GLchar) * glLogLength);
		glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &glLogLength);
		glGetProgramInfoLog(glProgram, glLogLength, NULL, glLog);
		printf("Error linking shader program: %s\n", glLog);
		free(glLog);
	} else {
		printf("Shader program compiled!\n");
	}

	glDeleteShader(glFragment);
	glDeleteShader(glVertex);

	if(glCompileStatus == GL_FALSE)
		glProgram = 0;
	return glProgram;

}

int mgl_compileShader(GLenum shader_type, char* source_file)
{
	// Load file
	mgl_shaderSource *src = mgl_loadShaderFile(source_file);
	
	// Compile
	printf("ok\n");
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &src->src, NULL);
	glCompileShader(shader);
	mgl_shaderSourceDelete(src);

	// Check for errors
	GLint comp_status = 0, log_length = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &comp_status);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
	if(comp_status == GL_FALSE)
	{
		GLchar* compile_log = malloc(sizeof(char) * log_length);
		glGetShaderInfoLog(shader, log_length, NULL, compile_log);
		printf(" ---- Compile log ----\n%s ----     end     ----\n", compile_log);
		free(compile_log);
	} else {
		printf("Shader '%s' compiled!\n", source_file);
	}

	return shader;
}

mgl_shaderSource* mgl_loadShaderFile(char* filename)
{
	printf("Loading shader file: %s\n", filename);
	FILE *fptr = fopen(filename, "r");
	if(fptr == NULL && errno != 0)
	{
		printf("Error oppening file (errno:%d) \"%s\".\n", errno, filename);
		return NULL;
	}

	fseek(fptr, 0, SEEK_END);
	fpos_t size = ftell(fptr) + 1; // account for null byte at the end
	rewind(fptr);
	char *source = malloc(size);

	fread(source, sizeof(char), size, fptr);
	source[size - 1] = '\0'; // add null byte at end of array
	fclose(fptr);

	mgl_shaderSource *s = malloc(sizeof(mgl_shaderSource));
	s->src = source;
	s->size = size;
	return s;
}

void mgl_shaderSourceDelete(mgl_shaderSource *shader)
{
	free(shader);
	return;
}