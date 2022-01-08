#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	bool useSpotRenderer = false;

	Window w("Make your own project!", 1920, 1080, true);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);

	if(!renderer.HasInitialised()) {
		return -1;
	}

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Q)) {
			useSpotRenderer = !useSpotRenderer;
		}

		if (!useSpotRenderer) {
			// switch camera auto move
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
				renderer.SwitchCameraAutoMove();
			}
			// switch sunlight auto move
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
				renderer.SwitchSunLightAutoMove();
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
				renderer.SwitchPostProcess();
			}
			if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
				renderer.SwitchSecondCamera();
			}

			//
			renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds(), false);
			renderer.RenderScene(false);
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
		else if (useSpotRenderer) {
			renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds(), true);
			renderer.RenderScene(true);
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
	}
	return 0;
}