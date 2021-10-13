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
	class RandomHeightmapGenerator final {
	public:
		RandomHeightmapGenerator(int mapWidth, int mapHeight);
		RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomHeightmapGenerationParameters& parameters);

		std::unique_ptr<LayeredHeightmap> generate(const std::vector<std::unique_ptr<LayerData>>& layerData) const;
		RandomGenerationControlParameters getDefaultRandomGenerationControlParameters() const;
		RandomHeightmapGenerationParameters getDefaultGenerationParameters() const;
		void setGenerationParameters(const RandomHeightmapGenerationParameters& parameters);

	private:
		int mapWidth;
		int mapHeight;
		RandomHeightmapGenerationParameters parameters;
		NoiseMapGenerator noiseMapGenerator;
		AbsoluteNoiseMapGenerator absoluteNoiseMapGenerator;
		MaskGenerator maskGenerator;

		std::unique_ptr<LayeredHeightmap> generateLayeredHeightmap(const std::vector<std::unique_ptr<LayerData>>& layerData) const;
		std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> generateLayers(const std::vector<std::unique_ptr<LayerData>>& layerData) const;
		std::unique_ptr<HeightmapLayer> generateNoiseLayer(const NoiseMapGenerator& generator, const LayerData& layerData) const;
		std::unique_ptr<Heightmap> generateFinalMask(const FinalMaskGenerationParameters& finalMaskParameters) const;
		void adjustLayeredHeightmap(LayeredHeightmap& heightmap) const;
	};
}