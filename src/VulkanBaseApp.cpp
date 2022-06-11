#include "VulkanBaseApp.h"

#include <vector>

namespace RenderingEngineApplication
{
	void VulkanApplication::Run()
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		CleanUp();
	}

	void VulkanApplication::InitWindow()
	{
		int status = glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_mWindow = glfwCreateWindow(_mWinWidth, _mWinHeight, "Vulkan", nullptr, nullptr);
	}

	void VulkanApplication::InitVulkan()
	{
		CreateVulkanInstance();
	}

	void VulkanApplication::MainLoop()
	{
		while (!glfwWindowShouldClose(_mWindow))
		{
			glfwPollEvents();
		}
	}

	void VulkanApplication::CleanUp()
	{
		vkDestroyInstance(_mInstance, nullptr);

		glfwDestroyWindow(_mWindow);

		glfwTerminate();
	}

	void VulkanApplication::CreateVulkanInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Application";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
		appInfo.pEngineName = "Rendering Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		uint32_t glfw_extensionCounts = 0;
		const char** glfw_extensionNames;

		glfw_extensionNames = glfwGetRequiredInstanceExtensions(&glfw_extensionCounts);

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensionProperties(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

		std::cout << "Extension Properties available are" << '\n';
		for(auto& extension : extensionProperties)
		{
			std::cout << extension.extensionName << "\n";
		}

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfw_extensionCounts;
		createInfo.ppEnabledExtensionNames = glfw_extensionNames;
		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &_mInstance);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to Create Instance");
		}
	}
}