#pragma once

#include <GL/glew.h>

#include "Image.hpp"

namespace pcb {
	class Texture final {
	public:
		Texture(Image* sourceImage);
		~Texture();

		void bind() const;

	private:
		GLuint id;
	};
}