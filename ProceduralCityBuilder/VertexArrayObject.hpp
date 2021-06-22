#pragma once

#include <GL/glew.h>

namespace pcb {
	class VertexArrayObject {
	public:
		VertexArrayObject();
		~VertexArrayObject();

		void bind() const;

	private:
		GLuint name;
	};
}