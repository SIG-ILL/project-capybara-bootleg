#include "ShaderProgram.hpp"

pcb::ShaderProgram::ShaderProgram(pcb::VertexShader& vertexShader, pcb::FragmentShader& fragmentShader) {
	name = glCreateProgram();
	vertexShader.attachToShaderProgram(name);
	fragmentShader.attachToShaderProgram(name);

	glBindAttribLocation(name, 0, "aPosition");
	glLinkProgram(name);
}

void pcb::ShaderProgram::use() const {
	glUseProgram(name);
}