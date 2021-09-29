#pragma once

#include <GL/glew.h>

#include "Heightmap.hpp"

namespace pcb {
	class Terrain {
	public:
		Terrain(const Heightmap& heightmap, double scale);
		Terrain(const Terrain& other);
		virtual ~Terrain();

		Terrain& operator=(const Terrain& other);

		GLfloat* getQuadsVertices() const;
		int getQuadsVertexCount() const;
		GLfloat* getQuadsColors() const;
		std::unique_ptr<Heightmap> generateHeightmap() const;
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