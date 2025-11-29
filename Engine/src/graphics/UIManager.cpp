#include "UIManager.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_sdl3.h>

void UIManager::initializeImGui(SDL_Window* sdlWindowPtr, ImGui_ImplVulkan_InitInfo& initInfoPtr)
{
	// Initialize ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Dear ImGui style
	ImGui_ImplSDL3_InitForVulkan(sdlWindowPtr);
	ImGui_ImplVulkan_Init(&initInfoPtr);
}


void UIManager::newFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void UIManager::render()
{
	ImGui::Render();
}

void UIManager::beginRecording()
{
	ImGui::Begin("TODO", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
}

void UIManager::endRecording()
{
	ImGui::End();
}

void UIManager::recordToCommandBuffer(VkCommandBuffer cmdBuffer)
{

}

