#pragma once

#include <GL/glew.h>

#include "HeightMap.hpp"

namespace pcb {
	class Terrain {
	public:
		Terrain(pcb::HeightMap* heightMap, double scale);
		~Terrain();

		GLfloat* getQuadsVertices();
		int getQuadsVertexCount();
		GLfloat* getQuadsColors();
		pcb::HeightMap* generateHeightMapNew();

	private:
		int gridWidthInVertices;
		int gridHeightInVertices;
		int quadsVertexCount;
		GLfloat* quadsVertexCoordinates;
		GLfloat* quadsColors;
	};
}