#pragma once

#include <vector>
#include <memory>

#include "LayeredHeightmap.hpp"
#include "NoiseGenerator.hpp"
#include "BoundsPair.hpp"
#include "NoiseMapGenerator.hpp"
#include "AbsoluteNoiseMapGenerator.hpp"

namespace pcb {
#pragma region Random_Generation_Control_Properties
	class RandomGenerationControlProperties final {
	public:
		BoundsPair<unsigned int> amountOfLayersBounds;
		double layerBaseNoiseFrequency;
		double layerNoiseFrequencyAdditionalLayerModifier;
		double layerScalingBaseValue;
		double layerScalingAdditionalLayerModifier;
		BoundsPair<int> noiseOffsetValueBounds;
		double defaultNoiseInversionChance;
		double absoluteNoiseChance;
		double absoluteNoiseFrequencyModifier;
		double absoluteNoiseInversionChance;

		double applyMaskOnLayerChance;
		BoundsPair<unsigned int> maskAmountOfLayersBounds;
		BoundsPair<double> maskOffsetMultiplicationValueBounds;
		BoundsPair<double> maskRadiusMultiplicationValueBounds;
		BoundsPair<double> maskFalloffMultiplicationValueBounds;
		double maskDefaultNoiseInversionChance;
		double maskAbsoluteNoiseChance;
		double maskAbsoluteNoiseFrequencyModifier;
		double maskAbsoluteNoiseInversionChance;

		double applyFinalMaskChance;
		BoundsPair<unsigned int> finalMaskAmountOfLayersBounds;
		BoundsPair<double> finalMaskOffsetMultiplicationValueBounds;
		BoundsPair<double> finalMaskRadiusMultiplicationValueBounds;
		BoundsPair<double> finalMaskFalloffMultiplicationValueBounds;
		BoundsPair<double> finalMaskCompositeMaskShapesDistanceMultiplierBounds;

		BoundsPair<unsigned int> adjustmentLoweringThresholds;
		unsigned int adjustmentLoweringValue;
		unsigned int adjustmentScaleDownAmplitudeThreshold;
		BoundsPair<double> adjustmentScaleDownAmplitudeValueBounds;
		unsigned int adjustmentScaleUpAmplitudeThreshold;
		BoundsPair<double> adjustmentScaleUpAmplitudeValueBounds;
	};
#pragma endregion

	class RandomHeightmapGenerator final {
	public:
		RandomHeightmapGenerator(int mapWidth, int mapHeight);
		RandomHeightmapGenerator(int mapWidth, int mapHeight, const RandomGenerationControlProperties& properties);

		std::unique_ptr<LayeredHeightmap> generate() const;
		RandomGenerationControlProperties getDefaultControlProperties() const;
		void setControlProperties(const RandomGenerationControlProperties& properties);

	private:
		int mapWidth;
		int mapHeight;
		RandomGenerationControlProperties properties;
		NoiseMapGenerator noiseMapGenerator;
		AbsoluteNoiseMapGenerator absoluteNoiseMapGenerator;

		std::unique_ptr<LayeredHeightmap> generateLayeredHeightmap() const;
		std::unique_ptr<std::vector<std::unique_ptr<HeightmapLayer>>> generateLayers() const;
		std::unique_ptr<std::vector<LayerMode>> generateLayerModes(int amountOfLayers) const;
		std::unique_ptr<HeightmapLayer> generateLayer(int layerIndex, LayerMode layerMode) const;
		int generateNoiseOffset() const;
		std::unique_ptr<Heightmap> generateMask() const;
		std::unique_ptr<Heightmap> generateFinalMask() const;
		void adjustLayeredHeightmap(LayeredHeightmap& heightmap) const;

#pragma region Inner_Classes
#pragma region Layer_Data
		class LayerData {
		public:
			std::unique_ptr<LayerData> getRandomAllowedNextNode(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const;
			LayerMode getMode() const;

		protected:
			LayerData(const LayerMode layerMode);

			virtual std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const = 0;
			int countAmountOfConsecutiveLayerModesAtEnd(const std::vector<std::unique_ptr<LayerData>>& previousLayers, pcb::LayerMode layerMode) const;

		private:
			LayerMode layerMode;
		};

		class AdditionLayerData final : public LayerData {
		public:
			AdditionLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
		};

		class SubtractionLayerData final : public LayerData {
		public:
			SubtractionLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
		};

		class MaskLayerData final : public LayerData {
		public:
			MaskLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
		};

		class FinalMaskLayerData final : public LayerData {
		public:
			FinalMaskLayerData();

		protected:
			std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
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
#pragma endregion
	};
}