#pragma once

#include <GL/glew.h>
#include <array>

#include "Texture.hpp"
#include "SimpleObject.hpp"
#include "TerrainGenerator.hpp"

namespace pcb {
	class Application {
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

		float translationX;
		float translationY;
		float rotationZ;
		float scale;

		Texture* heightmapTexture;
		Texture* generatedHeightmapTexture;
		SimpleObject* renderObjects[3];
		std::array<GLfloat*, 3> renderObjectsDataPointers;
		Terrain* terrain;

		void initializeGLUT(int argc, char* argv[]);
		void loadResources();

		void drawTestShapes();

		void render();
		void reshape(int width, int height);
		void handleKeyboard(unsigned char key, int x, int y);
		void idleUpdate();
	};
}