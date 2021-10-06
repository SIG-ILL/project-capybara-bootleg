#pragma once

#include <GL/glew.h>
#include <vector>

#include "Heightmap.hpp"

namespace pcb {
	class Terrain {
	public:
		Terrain(const Heightmap& heightmap, double scale);
		Terrain(const Heightmap& heightmap, double scale, GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation);

		std::shared_ptr<std::vector<GLfloat>> getQuadsVertices() const;
		int getQuadsVertexCount() const;
		std::shared_ptr<std::vector<GLfloat>> getQuadsColors() const;
		std::unique_ptr<Heightmap> generateHeightmap() const;
		void setHeightBasedColorGradient(GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation);

	private:
		int gridWidthInVertices;
		int gridHeightInVertices;
		int quadsVertexCount;
		std::shared_ptr<std::vector<GLfloat>> quadsVertexCoordinates;	// TODO: Changing coordinates and colors vectors to std::shared_ptr<GLfloat[]> can give a decent performance boost (~28% on debug builds), but is not as recommended as a vector.
		std::shared_ptr<std::vector<GLfloat>> quadsColors;
		GLfloat highestElevation;

		void setVertexDataValues(const Heightmap& heightmap, double scale, GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation);
		double getScale() const;
	};
}