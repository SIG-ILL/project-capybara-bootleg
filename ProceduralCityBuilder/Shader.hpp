#pragma once

#include <string>
#include <GL/glew.h>
#include <vector>

namespace pcb {
	class Shader {
	protected:
		Shader(std::string& sourceCode, GLenum type);

	public:
		void attachToShaderProgram(GLuint programName);

	private:
		GLuint name;
		std::vector<GLuint> associatedShaderPrograms;
	};

	class VertexShader : public Shader {
	public:
		VertexShader(std::string& sourceCode);
	};

	class FragmentShader : public Shader {
	public:
		FragmentShader(std::string& sourceCode);
	};
}