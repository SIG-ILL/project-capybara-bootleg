#pragma once

#include <GL/glew.h>

#include "Image.hpp"

namespace pcb {
	class Texture {
	public:
		Texture(Image* sourceImage);
		~Texture();

		void bind();

	private:
		GLuint id;
	};
}