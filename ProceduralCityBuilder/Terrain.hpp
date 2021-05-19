#pragma once

#include <GL/glew.h>

#include "Heightmap.hpp"

namespace pcb {
	class Terrain {
	public:
		Terrain(pcb::Heightmap* heightmap, double scale);
		~Terrain();

		GLfloat* getQuadsVertices();
		int getQuadsVertexCount();
		GLfloat* getQuadsColors();
		pcb::Heightmap* generateHeightmapNew();

	private:
		int gridWidthInVertices;
		int gridHeightInVertices;
		int quadsVertexCount;
		GLfloat* quadsVertexCoordinates;
		GLfloat* quadsColors;
	};
}