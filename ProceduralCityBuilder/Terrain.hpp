#pragma once

#include <GL/glew.h>

#include "Heightmap.hpp"

namespace pcb {
	class Terrain {
	public:
		Terrain(const pcb::Heightmap& heightmap, double scale);
		Terrain(const pcb::Terrain& other);
		virtual ~Terrain();

		pcb::Terrain& operator=(const pcb::Terrain& other);

		GLfloat* getQuadsVertices() const;
		int getQuadsVertexCount() const;
		GLfloat* getQuadsColors() const;
		pcb::Heightmap generateHeightmap() const;
		void setHeightBasedColorGradient(GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation);

	private:
		int gridWidthInVertices;
		int gridHeightInVertices;
		int quadsVertexCount;
		GLfloat* quadsVertexCoordinates;
		GLfloat* quadsColors;
		GLfloat highestElevation;

		double getScale() const;
	};
}