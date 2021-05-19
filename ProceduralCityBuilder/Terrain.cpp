#include "Terrain.hpp"

#include <cmath>

pcb::Terrain::Terrain(pcb::Heightmap* heightmap, double scale) : gridWidthInVertices(heightmap->getWidth()), gridHeightInVertices(heightmap->getHeight()), quadsVertexCount(4 * (heightmap->getWidth() - 1) * (heightmap->getHeight() - 1)), quadsVertexCoordinates(new GLfloat[3 * quadsVertexCount]), quadsColors(new GLfloat[3 * quadsVertexCount]) {
	// Array size = 4 * 3 * (width - 1) * (height - 1) (4 for 4 vertices per quad, 3 for 3 dimensions - 3 values per coordinate).
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			int index = (4 * 3 * y * maxLoopWidthIndex) + (4 * 3 * x);

			GLfloat elevation = static_cast<GLfloat>(heightmap->getValueAt(x, y));
			int vertexX = x;
			int vertexY = y;
			quadsVertexCoordinates[index] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 1] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 2] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x + 1;
			vertexY = y;
			elevation = static_cast<GLfloat>(heightmap->getValueAt(vertexX, vertexY));
			quadsVertexCoordinates[index + 3] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 4] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 5] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x + 1;
			vertexY = y + 1;
			elevation = static_cast<GLfloat>(heightmap->getValueAt(vertexX, vertexY));
			quadsVertexCoordinates[index + 6] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 7] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 8] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x;
			vertexY = y + 1;
			elevation = static_cast<GLfloat>(heightmap->getValueAt(vertexX, vertexY));
			quadsVertexCoordinates[index + 9] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 10] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 11] = static_cast<GLfloat>(scale * vertexY);
		}
	}

	for (int i = 0; i < (3 * quadsVertexCount); i += 3) {
		GLfloat elevation = quadsVertexCoordinates[i + 1];
		GLfloat colorValue = static_cast<GLfloat>((elevation / scale) / 255);
		quadsColors[i] = colorValue;
		quadsColors[i + 1] = colorValue;
		quadsColors[i + 2] = colorValue;
	}

	// A one-dimensional height map can't generate terrain as each pixel is used as vertex coordinates, not the definition of an entire quad.
	if (gridWidthInVertices == 1 || gridHeightInVertices == 1) {
		gridWidthInVertices = 0;
		gridWidthInVertices = 0;
	}
}

GLfloat* pcb::Terrain::getQuadsColors() {
	return quadsColors;
}

pcb::Terrain::~Terrain() {
	delete[] quadsVertexCoordinates;
	delete[] quadsColors;
}

GLfloat* pcb::Terrain::getQuadsVertices() {
	return quadsVertexCoordinates;
}

int pcb::Terrain::getQuadsVertexCount() {
	return quadsVertexCount;
}

pcb::Heightmap* pcb::Terrain::generateHeightmapNew() {
	unsigned char* elevationValues = new unsigned char[gridWidthInVertices * gridHeightInVertices];
	double scale = 1.0;
	if (gridWidthInVertices > 1 && gridHeightInVertices > 1) {
		scale = quadsVertexCoordinates[3];	// The original, pre-scaling value of the coordinate at this element was x coordinate = 1. The value was scaled and stored, but scale == (value / 1) -> scale == value.
	}

	double inverseScale = 1.0 / scale;
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			elevationValues[(y * gridWidthInVertices) + x] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * y * maxLoopWidthIndex) + (4 * 3 * x) + 1]), 255));
		}

		elevationValues[(y * gridWidthInVertices) + maxLoopWidthIndex] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * y * maxLoopWidthIndex) + (4 * 3 * (maxLoopWidthIndex - 1)) + 3 + 1]), 255));
	}

	for (int i = 0; i < maxLoopWidthIndex; i++) {
		elevationValues[((gridHeightInVertices - 1) * gridWidthInVertices) + i] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (4 * 3 * i) + (3 * 3) + 1]), 255));
	}

	elevationValues[(gridWidthInVertices * gridHeightInVertices) - 1] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (4 * 3 * (maxLoopWidthIndex - 1)) + (3 * 2) + 1]), 255));;

	pcb::Heightmap* heightmap = new pcb::Heightmap(gridWidthInVertices, gridHeightInVertices, elevationValues);
	delete[] elevationValues;

	return heightmap;
}