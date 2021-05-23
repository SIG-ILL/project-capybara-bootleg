#pragma once

#include <GL/glew.h>
#include <array>
#include <vector>

#include "Texture.hpp"
#include "SimpleObject.hpp"
#include "TerrainGenerator.hpp"

namespace pcb {
	class Application final {
	public:
		Application();
		~Application();

		void run(Application* instance, int argc, char* argv[]);

	private:
		static Application* instance;

		static void renderCallback();
		static void reshapeCallback(int width, int height);
		static void keyboardCallback(unsigned char key, int x, int y);
		static void idleCallback();
		static void mouseMotionCallback(int x, int y);
		static void mouseWheelCallback(int button, int dir, int x, int y);

		float translationX;
		float translationY;
		float rotationZ;
		float scale;
		int mouseWindowX;
		int mouseWindowY;
		int globalRotationX;
		int globalRotationY;
		bool isWarpingPointer;
		double zoom;

		Texture* heightmapTexture;
		Texture* generatedHeightmapTexture;
		std::array<SimpleObject*, 3> renderObjects;
		std::array<GLfloat*, 3> renderObjectsDataPointers;
		Terrain* terrain;
		std::vector<Terrain> terrainLayers;
		std::vector<SimpleColoredObject> terrainLayerRenderObjects;

		void initializeGLUT(int argc, char* argv[]);
		void loadResources();

		void drawTestShapes() const;

		void render() const;
		void reshape(int width, int height) const;
		void handleKeyboard(unsigned char key, int x, int y);
		void idleUpdate() const;
		void handleMouseMotion(int x, int y);
		void handleMouseWheel(int button, int dir, int x, int y);
	};
}