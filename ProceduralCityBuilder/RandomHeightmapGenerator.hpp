#pragma once

#include <vector>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"

namespace pcb {
	class RandomHeightmapGenerator final {
	public:
		RandomHeightmapGenerator(int mapWidth, int mapHeight);

		LayeredHeightmap* generateNew() const;

	private:
		int mapWidth;
		int mapHeight;
		pcb::NoiseGenerator noiseGenerator;

		std::vector<LayerMode> generateLayerModes(const int numberOfLayers) const;
		pcb::Heightmap generateHeightmap(double noiseSamplingFrequencyX, double noiseSamplingFrequencyY, double xOffset, double yOffset) const;
		double generateElevationForNoiseCoordinates(double x, double y) const;
		Heightmap generateMask() const;
		Heightmap generateFinalMask() const;

#pragma region Layer_Data
		class LayerData {
		public:
			const std::vector<LayerMode> getAllowedNextModes(const std::vector<LayerData*> previousLayers) const;
			const LayerMode getMode() const;

		protected:
			LayerData(const LayerMode layerMode);

			virtual std::vector<LayerMode> determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const = 0;
			int countAmountOfConsecutiveLayerModesAtEnd(const std::vector<LayerData*> previousLayers, pcb::LayerMode layerMode) const;

		private:
			LayerMode layerMode;
		};

		class AdditionLayerData final : public LayerData {
		public:
			AdditionLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const override;
		};

		class SubtractionLayerData final : public LayerData {
		public:
			SubtractionLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const override;
		};

		class MaskLayerData final : public LayerData {
		public:
			MaskLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<LayerData*> previousLayers) const override;
		};
#pragma endregion

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
	};
}