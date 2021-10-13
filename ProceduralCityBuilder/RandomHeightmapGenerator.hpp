#pragma once

#include <vector>
#include <memory>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"
#include "NoiseMapGenerator.hpp"
#include "AbsoluteNoiseMapGenerator.hpp"
#include "HeightmapGenerationProperties.hpp"
#include "RandomHeightmapLayerDataGenerator.hpp"
#include "MaskGenerator.hpp"

namespace pcb {
#pragma region Helper_Classes
#pragma region Composite_Mask_Shape_Data
	class MaskShapeData final {
	public:
		int offsetX;
		int offsetY;
		int unaffectedRadiusX;
		int unaffectedRadiusY;
		int falloffWidth;
	};
#pragma endregion
#pragma endregion

	class RandomHeightmapGenerator final {
	public:
		RandomHeightmapGenerator(int mapWidth, int mapHeight);
		RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomGenerationControlParameters& properties);

		std::unique_ptr<LayeredHeightmap> generate() const;
		RandomGenerationControlParameters getDefaultControlProperties() const;
		void setControlProperties(const RandomGenerationControlParameters& properties);

	private:
		int mapWidth;
		int mapHeight;
		RandomGenerationControlParameters properties;
		NoiseMapGenerator noiseMapGenerator;
		AbsoluteNoiseMapGenerator absoluteNoiseMapGenerator;
		MaskGenerator maskGenerator;

		std::unique_ptr<LayeredHeightmap> generateLayeredHeightmap() const;
		std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> generateLayers() const;
		std::unique_ptr<HeightmapLayer> generateNoiseLayer(const NoiseMapGenerator& generator, const LayerData& layerData) const;
		std::unique_ptr<Heightmap> generateFinalMask() const;
		void adjustLayeredHeightmap(LayeredHeightmap& heightmap) const;
	};
}