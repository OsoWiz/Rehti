#pragma once

#include <vulkan/vulkan.h>

// fwd decl
struct ImGui_ImplVulkan_InitInfo;
struct SDL_Window;

class UIManager
{
public:

	UIManager(SDL_Window* sdlWindow, ImGui_ImplVulkan_InitInfo& initInfoPtr);
	~UIManager();

	void newFrame();
	void render();

	void beginRecording();
	void endRecording();
	void recordToCommandBuffer(VkCommandBuffer cmdBuffer);

private:

};

