#pragma once

#include <GL/glew.h>

#include "Image.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class HeightMap {
	public:
		HeightMap(int width, int height, unsigned char* elevationValues);
		~HeightMap();

		Image* to24BitImageNew();
		unsigned char getValueAt(int x, int y);
		int getWidth();
		int getHeight();

	private:
		int width;
		int height;
		unsigned char* elevationValues;
	};

	class Terrain {
	public:
		Terrain(pcb::HeightMap* heightMap, double scale);
		~Terrain();

		GLfloat* getQuadsVertices();
		int getQuadsVertexCount();

	private:
		int quadsVertexCount;
		GLfloat* quadsVertices;
	};

	class TerrainGenerator {
	public:
		pcb::Terrain* generateNew();
	};
	
	class HeightMapGenerator {
	public:
		HeightMapGenerator(int width, int height);

		pcb::HeightMap* generateNew();

	private:
		int width;
		int height;
		NoiseGenerator noiseGenerator;

		void createInitialLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor);
		void addAdditiveLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor);
		void addSubtractiveLayer(unsigned char* noiseMap, double noiseModifier, double multiplier, double maxValueFactor);
		double generateValueForCoordinates(double x, double y);
	};
}