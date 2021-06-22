#pragma once

#include <string>
#include <GL/glew.h>
#include <vector>

namespace pcb {
	class Shader {
	protected:
		Shader(const std::string& sourceCode, GLenum type);

	public:
		void attachToShaderProgram(GLuint programName);

	private:
		GLuint name;
		std::vector<GLuint> associatedShaderPrograms;
	};

	class VertexShader : public Shader {
	public:
		VertexShader(const std::string& sourceCode);
	};

	class FragmentShader : public Shader {
	public:
		FragmentShader(const std::string& sourceCode);
	};
}