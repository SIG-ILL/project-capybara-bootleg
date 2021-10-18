#include "Terrain.hpp"

#include <cmath>
#include <algorithm>
#include <vector>

#include "Logger.hpp"
#include <GL/freeglut.h>

pcb::Terrain::Terrain(const pcb::Heightmap& heightmap, double scale) : Terrain(heightmap, scale, 0.0f, 0.0f, 0.0, 1.0f, 1.0f, 1.0f, false) {}

pcb::Terrain::Terrain(const Heightmap& heightmap, double scale, GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation) :
	gridWidthInVertices(heightmap.getWidth()), gridHeightInVertices(heightmap.getHeight()), quadsVertexCount(4 * (heightmap.getWidth() - 1) * (heightmap.getHeight() - 1)),
quadsVertexCoordinates(std::make_shared<std::vector<GLfloat>>(3 * quadsVertexCount, 0.0f)), quadsColors(std::make_shared<std::vector<GLfloat>>(3 * quadsVertexCount, 0.0f)),
highestElevation(static_cast<GLfloat>(scale * heightmap.getHighestElevation())) {
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

std::unique_ptr<pcb::Heightmap> pcb::Terrain::generateHeightmap() const {	
	std::unique_ptr<std::vector<unsigned char>> elevationValues = std::make_unique<std::vector<unsigned char>>(gridWidthInVertices * gridHeightInVertices, 0);

	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	const int VERTICES_PER_QUAD = 4;
	double scale = getScale();
	double inverseScale = 1.0 / scale;
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			int index = (y * gridWidthInVertices) + x;
			(*elevationValues)[index] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * y * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * x) + 1)), 255));
		}
		int index = (y * gridWidthInVertices) + maxLoopWidthIndex;
		(*elevationValues)[index] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * y * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopWidthIndex - 1)) + 3 + 1)), 255));
	}

	for (int i = 0; i < maxLoopWidthIndex; i++) {
		int index = (maxLoopHeightIndex * gridWidthInVertices) + i;
		(*elevationValues)[index] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * i) + (3 * 3) + 1)), 255));
	}
	int index = (gridHeightInVertices * gridWidthInVertices) - 1;
	(*elevationValues)[index] = static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates->at((VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * (maxLoopWidthIndex - 1)) + (3 * 2) + 1)), 255));

	return std::make_unique<Heightmap>(gridWidthInVertices, gridHeightInVertices, std::move(elevationValues));
}

void pcb::Terrain::setVertexDataValues(const Heightmap& heightmap, double scale, GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation) {
	const int COORDINATE_ELEMENTS_PER_VERTEX = 3;
	const int VERTICES_PER_QUAD = 4;

	// Array size = 4 * 3 * (width - 1) * (height - 1) (4 for 4 vertices per quad, 3 for 3 dimensions - 3 values per coordinate).
	const int MAX_LOOP_WIDTH_INDEX = gridWidthInVertices - 1;
	const int MAX_LOOP_HEIGHT_INDEX = gridHeightInVertices - 1;

	GLfloat highestElevationValueDivider = 255;	// TODO: Highest value of the unsigned char heightmap data. This is an assumption and should be changed to be actually read from the source heightmap, in case the type of the source data changes.
	if (scaleToHighestElevation) {
		highestElevationValueDivider = static_cast<GLfloat>(highestElevation / scale);
	}
	const GLfloat RED_RANGE = maxRed - minRed;
	const GLfloat GREEN_RANGE = maxGreen - minGreen;
	const GLfloat BLUE_RANGE = maxBlue - minBlue;

	for (int y = 0; y < MAX_LOOP_HEIGHT_INDEX; y++) {
		int vertexY = y;
		int vertexYPlusOne = y + 1;
		int indexRowStartIndex = (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * MAX_LOOP_WIDTH_INDEX * y);

		for (int x = 0; x < MAX_LOOP_WIDTH_INDEX; x++) {
			int index = indexRowStartIndex + (VERTICES_PER_QUAD * COORDINATE_ELEMENTS_PER_VERTEX * x);
			int vertexX = x;
			int vertexXPlusOne = x + 1;

			GLfloat elevation1 = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			GLfloat elevation2 = static_cast<GLfloat>(heightmap.getValueAt(vertexXPlusOne, vertexY));
			GLfloat elevation3 = static_cast<GLfloat>(heightmap.getValueAt(vertexXPlusOne, vertexYPlusOne));
			GLfloat elevation4 = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexYPlusOne));

			GLfloat scaledVertexX = static_cast<GLfloat>(scale * vertexX);
			GLfloat scaledVertexY = static_cast<GLfloat>(scale * vertexY);
			GLfloat scaledVertexXPlusOne = static_cast<GLfloat>(scale * vertexXPlusOne);
			GLfloat scaledVertexYPlusOne = static_cast<GLfloat>(scale * vertexYPlusOne);

			GLfloat scaledElevation1 = static_cast<GLfloat>(scale * elevation1);
			GLfloat scaledElevation2 = static_cast<GLfloat>(scale * elevation2);
			GLfloat scaledElevation3 = static_cast<GLfloat>(scale * elevation3);
			GLfloat scaledElevation4 = static_cast<GLfloat>(scale * elevation4);

			GLfloat colorValue1 = elevation1 / highestElevationValueDivider;
			GLfloat colorValue2 = elevation2 / highestElevationValueDivider;
			GLfloat colorValue3 = elevation3 / highestElevationValueDivider;
			GLfloat colorValue4 = elevation4 / highestElevationValueDivider;
			
			(*quadsVertexCoordinates)[index] = scaledVertexX;
			(*quadsVertexCoordinates)[index + 1] = scaledElevation1;
			(*quadsVertexCoordinates)[index + 2] = scaledVertexY;
			(*quadsVertexCoordinates)[index + 3] = scaledVertexXPlusOne;
			(*quadsVertexCoordinates)[index + 4] = scaledElevation2;
			(*quadsVertexCoordinates)[index + 5] = scaledVertexY;
			(*quadsVertexCoordinates)[index + 6] = scaledVertexXPlusOne;
			(*quadsVertexCoordinates)[index + 7] = scaledElevation3;
			(*quadsVertexCoordinates)[index + 8] = scaledVertexYPlusOne;
			(*quadsVertexCoordinates)[index + 9] = scaledVertexX;
			(*quadsVertexCoordinates)[index + 10] = scaledElevation4;
			(*quadsVertexCoordinates)[index + 11] = scaledVertexYPlusOne;
			
			(*quadsColors)[index] = minRed + (colorValue1 * RED_RANGE);
			(*quadsColors)[index + 1] = minGreen + (colorValue1 * GREEN_RANGE);
			(*quadsColors)[index + 2] = minBlue + (colorValue1 * BLUE_RANGE);
			(*quadsColors)[index + 3] = minRed + (colorValue2 * RED_RANGE);
			(*quadsColors)[index + 4] = minGreen + (colorValue2 * GREEN_RANGE);
			(*quadsColors)[index + 5] = minBlue + (colorValue2 * BLUE_RANGE);
			(*quadsColors)[index + 6] = minRed + (colorValue3 * RED_RANGE);
			(*quadsColors)[index + 7] = minGreen + (colorValue3 * GREEN_RANGE);
			(*quadsColors)[index + 8] = minBlue + (colorValue3 * BLUE_RANGE);
			(*quadsColors)[index + 9] = minRed + (colorValue4 * RED_RANGE);
			(*quadsColors)[index + 10] = minGreen + (colorValue4 * GREEN_RANGE);
			(*quadsColors)[index + 11] = minBlue + (colorValue4 * BLUE_RANGE);
		}
	}
}

double pcb::Terrain::getScale() const {
	double scale = 1.0;
	if (gridWidthInVertices > 1 && gridHeightInVertices > 1) {
		scale = quadsVertexCoordinates->at(3);	// The original, pre-scaling value of the coordinate at this element was x coordinate = 1. The value was scaled and stored, but scale == (value / 1) -> scale == value.
	}

	return scale;
}