#pragma once

#include <vector>
#include <memory>

#include "LayeredHeightmap.hpp"
#include "NoiseMapGenerator.hpp"
#include "AbsoluteNoiseMapGenerator.hpp"
#include "RandomHeightmapLayerDataGenerator.hpp"
#include "MaskGenerator.hpp"
#include "RandomHeightmapGenerationData.hpp"

namespace pcb {
	class RandomHeightmapGenerator final {
	public:
		RandomHeightmapGenerator(int mapWidth, int mapHeight);

		std::unique_ptr<LayeredHeightmap> generate(const RandomHeightmapGenerationData& generationData) const;

	private:
		int mapWidth;
		int mapHeight;
		NoiseMapGenerator noiseMapGenerator;
		AbsoluteNoiseMapGenerator absoluteNoiseMapGenerator;
		MaskGenerator maskGenerator;

		std::unique_ptr<LayeredHeightmap> generateLayeredHeightmap(const RandomHeightmapGenerationData& generationData) const;
		std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> generateLayers(const std::vector<std::unique_ptr<LayerData>>& layerData) const;
		std::unique_ptr<HeightmapLayer> generateNoiseLayer(const NoiseMapGenerator& generator, const LayerData& layerData) const;
		std::unique_ptr<Heightmap> generateFinalMask(const FinalMaskGenerationParameters& finalMaskParameters) const;
		void postProcessHeightmap(LayeredHeightmap& heightmap, const RandomHeightmapGenerationData& generationData) const;
	};
}