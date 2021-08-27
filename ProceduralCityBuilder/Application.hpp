#pragma once

#include <GL/glew.h>
#include <array>
#include <vector>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SimpleObject.hpp"
#include "TerrainGenerator.hpp"
#include "ShaderManager.hpp"

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
		static void mouseCallback(int button, int state, int x, int y);

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
		std::vector<Terrain> terrainLayers;
		std::vector<SimpleColoredObject> terrainLayerRenderObjects;
		std::vector<VertexBufferObject*> vbos;
		ShaderManager shaderManager;
		glm::mat4 projectionMatrix;

		int previousGlutElapsedTime;

		void initializeGLUT(int argc, char* argv[]);
		void loadResources();
		void generateTerrainResources();
		void deleteResources();
		void prepareShaders();
		std::string loadShaderFromFile(std::string filepath) const;

		void drawTestShapes() const;

		void render();
		void reshape(int width, int height);
		void handleKeyboard(unsigned char key, int x, int y);
		void idleUpdate() const;
		void handleMouseMotion(int x, int y);
		void handleMouseWheel(int button, int dir, int x, int y);
		void handleMouse(int button, int state, int x, int y);
	};
}