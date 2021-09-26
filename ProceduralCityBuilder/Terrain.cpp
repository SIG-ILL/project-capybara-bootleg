#include "Terrain.hpp"

#include <cmath>
#include <algorithm>
#include <vector>

pcb::Terrain::Terrain(const pcb::Heightmap& heightmap, double scale) : gridWidthInVertices(heightmap.getWidth()), gridHeightInVertices(heightmap.getHeight()), quadsVertexCount(4 * (heightmap.getWidth() - 1) * (heightmap.getHeight() - 1)), quadsVertexCoordinates(new GLfloat[3 * quadsVertexCount]), quadsColors(new GLfloat[3 * quadsVertexCount]), highestElevation(static_cast<GLfloat>(scale * heightmap.getHighestElevation())) {
	// Array size = 4 * 3 * (width - 1) * (height - 1) (4 for 4 vertices per quad, 3 for 3 dimensions - 3 values per coordinate).
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			int index = (4 * 3 * y * maxLoopWidthIndex) + (4 * 3 * x);

			GLfloat elevation = static_cast<GLfloat>(heightmap.getValueAt(x, y));
			int vertexX = x;
			int vertexY = y;
			quadsVertexCoordinates[index] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 1] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 2] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x + 1;
			vertexY = y;
			elevation = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			quadsVertexCoordinates[index + 3] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 4] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 5] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x + 1;
			vertexY = y + 1;
			elevation = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			quadsVertexCoordinates[index + 6] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 7] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 8] = static_cast<GLfloat>(scale * vertexY);

			vertexX = x;
			vertexY = y + 1;
			elevation = static_cast<GLfloat>(heightmap.getValueAt(vertexX, vertexY));
			quadsVertexCoordinates[index + 9] = static_cast<GLfloat>(scale * vertexX);
			quadsVertexCoordinates[index + 10] = static_cast<GLfloat>(scale * elevation);
			quadsVertexCoordinates[index + 11] = static_cast<GLfloat>(scale * vertexY);
		}
	}

	setHeightBasedColorGradient(0, 0, 0, 1, 1, 1, false);

	// A one-dimensional height map can't generate terrain as each pixel is used as vertex coordinates, not the definition of an entire quad.
	if (gridWidthInVertices == 1 || gridHeightInVertices == 1) {
		gridWidthInVertices = 0;
		gridWidthInVertices = 0;
	}
}

pcb::Terrain::Terrain(const pcb::Terrain& other) : gridWidthInVertices(other.gridWidthInVertices), gridHeightInVertices(other.gridHeightInVertices), quadsVertexCount(other.quadsVertexCount), quadsVertexCoordinates(new GLfloat[3 * quadsVertexCount]), quadsColors(new GLfloat[3 * quadsVertexCount]), highestElevation(other.highestElevation) {
	std::copy(other.quadsVertexCoordinates, other.quadsVertexCoordinates + (3 * other.quadsVertexCount), quadsVertexCoordinates);
	std::copy(other.quadsColors, other.quadsColors + (3 * other.quadsVertexCount), quadsColors);
}

pcb::Terrain::~Terrain() {
	delete[] quadsVertexCoordinates;
	delete[] quadsColors;
}

pcb::Terrain& pcb::Terrain::operator=(const pcb::Terrain& other) {
	if (this == &other) {
		return *this;
	}

	gridWidthInVertices = other.gridWidthInVertices;
	gridHeightInVertices = other.gridHeightInVertices;
	quadsVertexCount = other.quadsVertexCount;

	if (quadsVertexCoordinates != other.quadsVertexCoordinates) {
		delete[] quadsVertexCoordinates;
		delete[] quadsColors;
		quadsVertexCoordinates = new GLfloat[3 * other.quadsVertexCount];
		quadsColors = new GLfloat[3 * other.quadsVertexCount];
	}

	std::copy(other.quadsVertexCoordinates, other.quadsVertexCoordinates + (3 * other.quadsVertexCount), quadsVertexCoordinates);
	std::copy(other.quadsColors, other.quadsColors + (3 * other.quadsVertexCount), quadsColors);

	return *this;
}

GLfloat* pcb::Terrain::getQuadsVertices() const {
	return quadsVertexCoordinates;
}

int pcb::Terrain::getQuadsVertexCount() const {
	return quadsVertexCount;
}

GLfloat* pcb::Terrain::getQuadsColors() const {
	return quadsColors;
}

pcb::Heightmap pcb::Terrain::generateHeightmap() const {
	double scale = getScale();
	std::vector<unsigned char> elevationValues;
	elevationValues.reserve(gridWidthInVertices * gridHeightInVertices);

	double inverseScale = 1.0 / scale;
	int maxLoopWidthIndex = gridWidthInVertices - 1;
	int maxLoopHeightIndex = gridHeightInVertices - 1;

	for (int y = 0; y < maxLoopHeightIndex; y++) {
		for (int x = 0; x < maxLoopWidthIndex; x++) {
			elevationValues.push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * y * maxLoopWidthIndex) + (4 * 3 * x) + 1]), 255)));
		}
		elevationValues.push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * y * maxLoopWidthIndex) + (4 * 3 * (maxLoopWidthIndex - 1)) + 3 + 1]), 255)));
	}

	for (int i = 0; i < maxLoopWidthIndex; i++) {
		elevationValues.push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (4 * 3 * i) + (3 * 3) + 1]), 255)));
	}
	elevationValues.push_back(static_cast<unsigned char>(std::fmin(std::round(inverseScale * quadsVertexCoordinates[(4 * 3 * (maxLoopHeightIndex - 1) * maxLoopWidthIndex) + (4 * 3 * (maxLoopWidthIndex - 1)) + (3 * 2) + 1]), 255)));

	pcb::Heightmap heightmap(gridWidthInVertices, gridHeightInVertices, elevationValues);

	return heightmap;
}

void pcb::Terrain::setHeightBasedColorGradient(GLfloat minRed, GLfloat minGreen, GLfloat minBlue, GLfloat maxRed, GLfloat maxGreen, GLfloat maxBlue, bool scaleToHighestElevation) {
	double scale = getScale();
	GLfloat highestElevationValueDivider = 255;	// TODO: Highest value of the unsigned char heightmap data. This is an assumption and should be changed to be actually read from the source heightmap, in case the type of the source data changes.
	if (scaleToHighestElevation) {
		highestElevationValueDivider = static_cast<GLfloat>(highestElevation / scale);
	}

	for (int i = 0; i < (3 * quadsVertexCount); i += 3) {
		GLfloat elevation = quadsVertexCoordinates[i + 1];
		GLfloat colorValue = static_cast<GLfloat>((elevation / scale) / highestElevationValueDivider);
		quadsColors[i] = minRed + (colorValue * (maxRed - minRed));
		quadsColors[i + 1] = minGreen + (colorValue * (maxGreen - minGreen));
		quadsColors[i + 2] = minBlue + (colorValue * (maxBlue - minBlue));
	}
}

double pcb::Terrain::getScale() const {
	double scale = 1.0;
	if (gridWidthInVertices > 1 && gridHeightInVertices > 1) {
		scale = quadsVertexCoordinates[3];	// The original, pre-scaling value of the coordinate at this element was x coordinate = 1. The value was scaled and stored, but scale == (value / 1) -> scale == value.
	}

	return scale;
}