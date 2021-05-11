#pragma once

namespace pcb {
	class Application {
	public:
		Application();

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

		void initializeGLUT(int argc, char* argv[]);
		void render();
		void reshape(int width, int height);
		void handleKeyboard(unsigned char key, int x, int y);
		void idleUpdate();
	};
}