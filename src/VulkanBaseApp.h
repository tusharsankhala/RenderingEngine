#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <exception>
#include <stdexcept>

namespace RenderingEngineApplication
{
	class VulkanApplication
	{
	public:
		void Run();
		void InitWindow();
		void InitVulkan();
		void SetupDebugMessenger();
		void PickPhysicalDevice();
		void CreateVulkanInstance();
		void MainLoop();
		void CleanUp();

	private:
		uint32_t _mWinWidth = 800;
		uint32_t _mWinHeight = 600;

		VkInstance					_mInstance;
		VkDebugUtilsMessengerEXT	_mDebugMessenger;
		VkPhysicalDevice			_mPhysicalDevice = VK_NULL_HANDLE;

		GLFWwindow* _mWindow;
	};
}