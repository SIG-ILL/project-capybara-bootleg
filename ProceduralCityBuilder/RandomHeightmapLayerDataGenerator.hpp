#pragma once

#include <vector>
#include <memory>

#include "HeightmapLayer.hpp"
#include "HeightmapGenerationProperties.hpp"

namespace pcb {
#pragma region Layer_Data
	class LayerData {
	public:
		std::unique_ptr<LayerData> getRandomAllowedNextNode(const std::vector<std::unique_ptr<LayerData>>& previousLayers, int layerIndex, const RandomGenerationControlParameters& properties) const;
		int getIndex() const;
		LayerMode getMode() const;
		NoiseMapGenerationParameters getNoiseMapGenerationParameters() const;
		FinalMaskGenerationParameters getFinalMaskGenerationParameters() const;

	protected:
		int width;
		int height;
		FinalMaskGenerationParameters finalMaskGenerationParameters;

		LayerData(int width, int height, int layerIndex, LayerMode layerMode, const RandomGenerationControlParameters& properties);

		virtual std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const = 0;
		int countAmountOfConsecutiveLayerModesAtEnd(const std::vector<std::unique_ptr<LayerData>>& previousLayers, pcb::LayerMode layerMode) const;

	private:
		int layerIndex;
		LayerMode layerMode;
		NoiseMapGenerationParameters generationParameters;

		void generateNoiseMapGenerationParameters(const RandomGenerationControlParameters& properties);
		int generateNoiseOffset(const RandomGenerationControlParameters& properties) const;
	};

	class AdditionLayerData final : public LayerData {
	public:
		AdditionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
	};

	class SubtractionLayerData final : public LayerData {
	public:
		SubtractionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
	};

	class MaskLayerData final : public LayerData {
	public:
		MaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
	};

	class FinalMaskLayerData final : public LayerData {
	public:
		FinalMaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& properties);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;

	private:
		void generateFinalMaskGenerationParameters(const RandomGenerationControlParameters& properties);
	};
#pragma endregion

	class RandomHeightmapLayerDataGenerator final {
	public:
		std::unique_ptr<std::vector<std::unique_ptr<LayerData>>> generateLayerData(int width, int height, int amountOfLayers, const RandomGenerationControlParameters& properties) const;
	};
}