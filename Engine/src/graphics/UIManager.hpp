#pragma once

#include <vulkan/vulkan.h>

// fwd decl
struct ImGui_ImplVulkan_InitInfo;
struct SDL_Window;

class UIManager
{
public:
	static void initializeImGui(SDL_Window* sdlWindow, ImGui_ImplVulkan_InitInfo& initInfoPtr);

	static UIManager* getInstance()
	{
		if (instance == nullptr)
		{
			instance = new UIManager();
		}
		return instance;
	}

	void newFrame();

	void render();

	void beginRecording();
	void endRecording();

	void recordToCommandBuffer(VkCommandBuffer cmdBuffer);

private:

	static UIManager* instance;
};

