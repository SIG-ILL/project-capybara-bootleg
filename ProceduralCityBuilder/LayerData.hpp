#pragma once

#include <memory>
#include <vector>

#include "HeightmapGenerationProperties.hpp"
#include "RandomGenerationControlParameters.hpp"
#include "HeightmapLayer.hpp"

namespace pcb {
#pragma region Layer_Data
	class LayerData {
	public:
		virtual FinalMaskGenerationParameters getFinalMaskGenerationParameters() const;

		std::unique_ptr<LayerData> getRandomAllowedNextNode(const std::vector<std::unique_ptr<LayerData>>& previousLayers, int layerIndex, const RandomGenerationControlParameters& controlParameters) const;
		int getIndex() const;
		LayerMode getMode() const;
		NoiseMapGenerationParameters getNoiseMapGenerationParameters() const;

	protected:
		int width;
		int height;

		LayerData(int width, int height, int layerIndex, LayerMode layerMode, const RandomGenerationControlParameters& controlParameters);

		virtual std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const = 0;
		int countAmountOfConsecutiveLayerModesAtEnd(const std::vector<std::unique_ptr<LayerData>>& previousLayers, pcb::LayerMode layerMode) const;

	private:
		int layerIndex;
		LayerMode layerMode;
		NoiseMapGenerationParameters generationParameters;

		void generateNoiseMapGenerationParameters(const RandomGenerationControlParameters& controlParameters);
		int generateNoiseOffset(const RandomGenerationControlParameters& controlParameters) const;
		CombinedMaskGenerationParameters generateNoiseMapMaskParameters(const RandomGenerationControlParameters& controlParameters) const;
		std::vector<GradientDirection> removeOppositeGradientDirection(GradientDirection direction, const std::vector<GradientDirection>& directionsList) const;
	};

	class AdditionLayerData final : public LayerData {
	public:
		AdditionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
	};

	class SubtractionLayerData final : public LayerData {
	public:
		SubtractionLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
	};

	class MaskLayerData final : public LayerData {
	public:
		MaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters);

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;
	};

	class FinalMaskLayerData final : public LayerData {
	public:
		FinalMaskLayerData(int width, int height, int layerIndex, const RandomGenerationControlParameters& controlParameters);

		FinalMaskGenerationParameters getFinalMaskGenerationParameters() const override;

	protected:
		std::vector<LayerMode> determineAllowedNextModes(const std::vector<std::unique_ptr<LayerData>>& previousLayers) const override;

	private:
		FinalMaskGenerationParameters finalMaskGenerationParameters;

		void generateFinalMaskGenerationParameters(const RandomGenerationControlParameters& controlParameters);
	};
#pragma endregion
}