#include "Terrain.hpp"

#include <cmath>
#include <algorithm>
#include <vector>

#include "Logger.hpp"
#include <GL/freeglut.h>

pcb::Terrain::Terrain(const pcb::Heightmap& heightmap, double scale) : Terrain(heightmap, scale, 0.0f, 0.0f, 0.0, 1.0f, 1.0f, 1.0f, false) {}

pcb::Terrain::Terrain(const Heightmap& heightmap, double scale, GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation) :
	gridWidthInVertices(heightmap.getWidth()), gridHeightInVertices(heightmap.getHeight()), quadsVertexCount(gridWidthInVertices * gridHeightInVertices),
quadsIndexCount(4 * (heightmap.getWidth() - 1) * (heightmap.getHeight() - 1)), quadsVertexCoordinates(std::make_shared<std::vector<GLfloat>>(3 * quadsVertexCount, 0.0f)), quadsColors(std::make_shared<std::vector<GLfloat>>(3 * quadsVertexCount, 0.0f)),
quadsIndices(std::make_shared<std::vector<unsigned int>>(quadsIndexCount, 0)), highestElevation(static_cast<GLfloat>(scale * heightmap.getHighestElevation())) {
	int glutElapsedTimeAtStart = glutGet(GLUT_ELAPSED_TIME);	

	setVertexDataValues(heightmap, scale, minRed, minGreen, minBlue, maxRed, maxGreen, maxBlue, scaleToHighestElevation);

	// A one-dimensional height map can't generate terrain as each pixel is used as vertex coordinates, not the definition of an entire quad.
	if (gridWidthInVertices == 1 || gridHeightInVertices == 1) {
		gridWidthInVertices = 0;
		gridWidthInVertices = 0;
	}

	int glutElapsedTimeAtEnd = glutGet(GLUT_ELAPSED_TIME);
	int milisecondsSinceLastTimeCheck = glutElapsedTimeAtEnd - glutElapsedTimeAtStart;
	Logger logger;
	logger << "Terrain generation took " << milisecondsSinceLastTimeCheck << "ms!\n";
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

std::shared_ptr<std::vector<unsigned int>> pcb::Terrain::getQuadsIndices() const {
	return quadsIndices;
}

int pcb::Terrain::getQuadsIndexCount() const {
	return quadsIndexCount;
}

std::unique_ptr<pcb::Heightmap> pcb::Terrain::generateHeightmap() const {
	std::unique_ptr<std::vector<unsigned char>> elevationValues = std::make_unique<std::vector<unsigned char>>(gridWidthInVertices * gridHeightInVertices, 0);

	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	const int VERTICES_PER_QUAD = 4;
	const int ELEVATION_OFFSET_IN_COORDINATES = 1;
	const int AMOUNT_OF_QUADS_HORIZONTALLY = gridWidthInVertices - 1;
	const int AMOUNT_OF_QUADS_VERTICALLY = gridHeightInVertices - 1;
	double scale = getScale();
	double inverseScale = 1.0 / scale;

	for (int y = 0; y < AMOUNT_OF_QUADS_VERTICALLY; y++) {
		int elevationIndexRowStartIndex = y * gridWidthInVertices;
		int indicesIndexRowStartIndex = y * AMOUNT_OF_QUADS_HORIZONTALLY * VERTICES_PER_QUAD;

		for (int x = 0; x < AMOUNT_OF_QUADS_HORIZONTALLY; x++) {
			int vertexIndex = (*quadsIndices)[indicesIndexRowStartIndex + (x * VERTICES_PER_QUAD)];
			GLfloat elevation = (*quadsVertexCoordinates)[(vertexIndex * COORDINATE_ELEMENTS_PER_VERTEX) + ELEVATION_OFFSET_IN_COORDINATES];
			elevation *= inverseScale;
			elevation = std::fmin(std::round(elevation), 255);
			(*elevationValues)[elevationIndexRowStartIndex + x] = elevation;
		}

		int vertexIndex = (*quadsIndices)[indicesIndexRowStartIndex + ((AMOUNT_OF_QUADS_HORIZONTALLY - 1) * VERTICES_PER_QUAD) + 1];
		GLfloat elevation = (*quadsVertexCoordinates)[(vertexIndex * COORDINATE_ELEMENTS_PER_VERTEX) + ELEVATION_OFFSET_IN_COORDINATES];
		elevation *= inverseScale;
		elevation = std::fmin(std::round(elevation), 255);
		(*elevationValues)[elevationIndexRowStartIndex + AMOUNT_OF_QUADS_HORIZONTALLY] = elevation;
	}

	int elevationIndexRowStartIndex = (gridHeightInVertices - 1) * gridWidthInVertices;
	int indicesIndexRowStartIndex = (AMOUNT_OF_QUADS_VERTICALLY - 1) * AMOUNT_OF_QUADS_HORIZONTALLY * VERTICES_PER_QUAD;

	for (int i = 0; i < AMOUNT_OF_QUADS_HORIZONTALLY; i++) {
		int vertexIndex = (*quadsIndices)[(indicesIndexRowStartIndex + (i * VERTICES_PER_QUAD)) + 3];
		GLfloat elevation = (*quadsVertexCoordinates)[(vertexIndex * COORDINATE_ELEMENTS_PER_VERTEX) + ELEVATION_OFFSET_IN_COORDINATES];
		elevation *= inverseScale;
		elevation = std::fmin(std::round(elevation), 255);
		(*elevationValues)[elevationIndexRowStartIndex + i] = elevation;
	}

	int vertexIndex = (*quadsIndices)[(indicesIndexRowStartIndex + ((AMOUNT_OF_QUADS_HORIZONTALLY - 1) * VERTICES_PER_QUAD)) + 2];
	GLfloat elevation = (*quadsVertexCoordinates)[(vertexIndex * COORDINATE_ELEMENTS_PER_VERTEX) + ELEVATION_OFFSET_IN_COORDINATES];
	elevation *= inverseScale;
	elevation = std::fmin(std::round(elevation), 255);
	(*elevationValues)[elevationIndexRowStartIndex + AMOUNT_OF_QUADS_HORIZONTALLY] = elevation;

	return std::make_unique<Heightmap>(gridWidthInVertices, gridHeightInVertices, std::move(elevationValues));
}

void pcb::Terrain::setVertexDataValues(const Heightmap& heightmap, double scale, GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation) {
	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	const GLfloat RED_RANGE = maxRed - minRed;
	const GLfloat GREEN_RANGE = maxGreen - minGreen;
	const GLfloat BLUE_RANGE = maxBlue - minBlue;

	GLfloat highestElevationValueDivider = 255;	// TODO: Highest value of the unsigned char heightmap data. This is an assumption and should be changed to be actually read from the source heightmap, in case the type of the source data changes.
	if (scaleToHighestElevation) {
		highestElevationValueDivider = static_cast<GLfloat>(highestElevation / scale);
	}

	for (int y = 0; y < gridHeightInVertices; y++) {
		int indexRowStartIndex = y * gridWidthInVertices * COORDINATE_ELEMENTS_PER_VERTEX;

		for (int x = 0; x < gridWidthInVertices; x++) {
			int index = indexRowStartIndex + (x * COORDINATE_ELEMENTS_PER_VERTEX);
			
			GLfloat elevation = static_cast<GLfloat>(heightmap.getValueAt(x, y));
			GLfloat scaledElevation = static_cast<GLfloat>(scale * elevation);
			GLfloat scaledVertexX = static_cast<GLfloat>(scale * x);
			GLfloat scaledVertexY = static_cast<GLfloat>(scale * y);
			GLfloat colorValue = elevation / highestElevationValueDivider;

			(*quadsVertexCoordinates)[index] = scaledVertexX;
			(*quadsVertexCoordinates)[index + 1] = scaledElevation;
			(*quadsVertexCoordinates)[index + 2] = scaledVertexY;

			(*quadsColors)[index] = minRed + (colorValue * RED_RANGE);
			(*quadsColors)[index + 1] = minGreen + (colorValue * GREEN_RANGE);
			(*quadsColors)[index + 2] = minBlue + (colorValue * BLUE_RANGE);
		}
	}

	const int VERTICES_PER_QUAD = 4;
	int amountOfQuads = quadsIndexCount / VERTICES_PER_QUAD;
	float amountOfQuadsHorizontally = gridWidthInVertices - 1;
	for (int i = 0; i < amountOfQuads; i++) {
		int index = 4 * i;
		int valueOfFirstIndex = i + std::floor(i / amountOfQuadsHorizontally);
		int valueOfSecondIndex = valueOfFirstIndex + 1;

		(*quadsIndices)[index] = valueOfFirstIndex;
		(*quadsIndices)[index + 1] = valueOfSecondIndex;
		(*quadsIndices)[index + 2] = valueOfSecondIndex + gridWidthInVertices;
		(*quadsIndices)[index + 3] = valueOfFirstIndex + gridWidthInVertices;
	}
}

double pcb::Terrain::getScale() const {
	double scale = 1.0;
	if (gridWidthInVertices > 1 && gridHeightInVertices > 1) {
		scale = quadsVertexCoordinates->at(3);	// The original, pre-scaling value of the coordinate at this element was x coordinate = 1. The value was scaled and stored, but scale == (value / 1) -> scale == value.
	}

	return scale;
}