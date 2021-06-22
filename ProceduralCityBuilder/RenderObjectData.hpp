#pragma once

#include <GL/glew.h>

#include "VertexArrayObject.hpp"
#include "ShaderProgram.hpp"

namespace pcb {
	class RenderObjectData {
	public:
		RenderObjectData(const pcb::VertexArrayObject& vertexArrayObject, const pcb::ShaderProgram& shaderProgram);

	private:
		const pcb::VertexArrayObject& vertexArrayObject;
		const pcb::ShaderProgram& shaderProgram;
	};
}