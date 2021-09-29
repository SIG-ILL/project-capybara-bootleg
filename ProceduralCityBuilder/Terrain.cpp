#include "Terrain.hpp"

#include <cmath>
#include <algorithm>
#include <vector>

pcb::Terrain::Terrain(const pcb::Heightmap& heightmap, double scale) : gridWidthInVertices(heightmap.getWidth()), gridHeightInVertices(heightmap.getHeight()), quadsVertexCount(4 * (heightmap.getWidth() - 1) * (heightmap.getHeight() - 1)), quadsVertexCoordinates(std::make_shared<std::vector<GLfloat>>(3 * quadsVertexCount, 0.0f)), quadsColors(std::make_shared<std::vector<GLfloat>>(3 * quadsVertexCount, 0.0f)), highestElevation(static_cast<GLfloat>(scale * heightmap.getHighestElevation())) {
	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	const int VERTICES_PER_QUAD = 4;

	// Array size = 4 * 3 * (width - 1) * (height - 1) (4 for 4 vertices per quad, 3 for 3 dimensions - 3 values per coordinate).
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			int index = (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * maxLoopWidthIndex * y) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * x);
			int vertexX = x;
			int vertexY = y;
			GLfloat elevation = static_cast<GLfloat>(heightmap.getValueAt(x, y));
			(*quadsVertexCoordinates)[index] = static_cast<GLfloat>(scale * vertexX);
			(*quadsVertexCoordinates)[index + 1] = static_cast<GLfloat>(scale * elevation);
			(*quadsVertexCoordinates)[index + 2] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x + 1;
			vertexY = y;
			elevation = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			(*quadsVertexCoordinates)[index + 3] = static_cast<GLfloat>(scale * vertexX);
			(*quadsVertexCoordinates)[index + 4] = static_cast<GLfloat>(scale * elevation);
			(*quadsVertexCoordinates)[index + 5] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x + 1;
			vertexY = y + 1;
			elevation = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			(*quadsVertexCoordinates)[index + 6] = static_cast<GLfloat>(scale * vertexX);
			(*quadsVertexCoordinates)[index + 7] = static_cast<GLfloat>(scale * elevation);
			(*quadsVertexCoordinates)[index + 8] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x;
			vertexY = y + 1;
			elevation = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			(*quadsVertexCoordinates)[index + 9] = static_cast<GLfloat>(scale * vertexX);
			(*quadsVertexCoordinates)[index + 10] = static_cast<GLfloat>(scale * elevation);
			(*quadsVertexCoordinates)[index + 11] = static_cast<GLfloat>(scale * vertexY);
		}
	}

	setHeightBasedColorGradient(0, 0, 0, 1, 1, 1, false);

	// A one-dimensional height map can't generate terrain as each pixel is used as vertex coordinates, not the definition of an entire quad.
	if (gridWidthInVertices == 1 || gridHeightInVertices == 1) {
		gridWidthInVertices = 0;
		gridWidthInVertices = 0;
	}
}

std::shared_ptr<std::vector<GLfloat>> pcb::Terrain::getQuadsVertices() const {
	return quadsVertexCoordinates;
}

int pcb::Terrain::getQuadsVertexCount() const {
	return quadsVertexCount;
}

std::shared_ptr<std::vector<GLfloat>> pcb::Terrain::getQuadsColors() const {
	return quadsColors;
}

std::unique_ptr<pcb::Heightmap> pcb::Terrain::generateHeightmap() const {	
	std::unique_ptr<std::vector<unsigned char>> elevationValues = std::make_unique<std::vector<unsigned char>>();
	elevationValues->reserve(gridWidthInVertices * gridHeightInVertices);

	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	const int VERTICES_PER_QUAD = 4;
	double scale = getScale();
	double inverseScale = 1.0 / scale;
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			elevationValues->push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * y * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * x) + 1)), 255)));
		}
		elevationValues->push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * y * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopWidthIndex - 1)) + 3 + 1)), 255)));
	}

	for (int i = 0; i < maxLoopWidthIndex; i++) {
		elevationValues->push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * i) + (3 * 3) + 1)), 255)));
	}
	elevationValues->push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopWidthIndex - 1)) + (3 * 2) + 1)), 255)));

	return std::make_unique<Heightmap>(gridWidthInVertices, gridHeightInVertices, std::move(elevationValues));
}

void pcb::Terrain::setHeightBasedColorGradient(GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation) {
	double scale = getScale();
	GLfloat highestElevationValueDivider = 255;	// TODO: Highest value of the unsigned char heightmap data. This is an assumption and should be changed to be actually read from the source heightmap, in case the type of the source data changes.
	if (scaleToHighestElevation) {
		highestElevationValueDivider = static_cast<GLfloat>(highestElevation / scale);
	}

	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	for (int i = 0; i < (COORDINATE_ELEMENTS_PER_VERTEX * quadsVertexCount); i += COORDINATE_ELEMENTS_PER_VERTEX) {
		GLfloat elevation = quadsVertexCoordinates->at(i + 1);
		GLfloat colorValue = static_cast<GLfloat>((elevation / scale) / highestElevationValueDivider);
		(*quadsColors)[i] = minRed + (colorValue * (maxRed - minRed));
		(*quadsColors)[i + 1] = minGreen + (colorValue * (maxGreen - minGreen));
		(*quadsColors)[i + 2] = minBlue + (colorValue * (maxBlue - minBlue));
	}
}

double pcb::Terrain::getScale() const {
	double scale = 1.0;
	if (gridWidthInVertices > 1 && gridHeightInVertices > 1) {
		scale = quadsVertexCoordinates->at(3);	// The original, pre-scaling value of the coordinate at this element was x coordinate = 1. The value was scaled and stored, but scale == (value / 1) -> scale == value.
	}

	return scale;
}