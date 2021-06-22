#include "Shader.hpp"

pcb::Shader::Shader(const std::string& sourceCode, GLenum type) : associatedShaderPrograms() {
	name = glCreateShader(type);
	const char* shaderSourceCString = sourceCode.data();	
	glShaderSource(name, 1, &shaderSourceCString, nullptr);
	glCompileShader(name);
}

void pcb::Shader::attachToShaderProgram(GLuint programName) {
	glAttachShader(programName, name);
	associatedShaderPrograms.push_back(programName);
}

pcb::VertexShader::VertexShader(const std::string& sourceCode) : Shader(sourceCode, GL_VERTEX_SHADER) {}

pcb::FragmentShader::FragmentShader(const std::string& sourceCode) : Shader(sourceCode, GL_FRAGMENT_SHADER) {}