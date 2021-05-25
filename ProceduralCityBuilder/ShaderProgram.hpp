#pragma once

#include <GL/glew.h>

#include "Shader.hpp"

namespace pcb {
	class ShaderProgram {
	public:
		ShaderProgram(pcb::VertexShader& vertexShader, pcb::FragmentShader& fragmentShader);

		void use() const;

	private:
		GLuint name;
	};
}