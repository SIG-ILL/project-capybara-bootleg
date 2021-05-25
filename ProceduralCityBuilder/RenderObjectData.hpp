#pragma once

#include <GL/glew.h>

#include "GLBufferObject.hpp"
#include "ShaderProgram.hpp"

namespace pcb {
	class RenderObjectData {
	public:
		RenderObjectData(const pcb::GLBufferObject& vertices, const pcb::ShaderProgram& shaderProgram);

	private:
		const pcb::GLBufferObject& vertices;
		const pcb::ShaderProgram& shaderProgram;
	};
}