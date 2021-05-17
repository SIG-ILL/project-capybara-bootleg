#pragma once

#include "Image.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class TerrainGenerator {
	public:
		void generate();
	};	

	/*
	8-bit greyscale heightmap.
	Elevation data is stored in a single array containing consecutive rows of elevation values.
	*/
	class HeightMap {
	public:
		HeightMap(int width, int height, unsigned char* elevationValues);
		~HeightMap();

		void print();
		Image* to24BitImage();

	private:
		int width;
		int height;
		unsigned char* elevationValues;
	};

	class HeightMapGenerator {
	public:
		HeightMapGenerator(int width, int height);

		pcb::HeightMap* generate();

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