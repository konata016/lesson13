#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

//画面サイズ
#define WIDTH 800
#define HEIGHT 600
#define enableValidationLayers false

class MyApplication{

private:
	GLFWwindow* window_;
	
	//初期化
	void InitializeWindow();
	void InitializeVulkan();

	void MainLoop();

	//片付け
	void FinalizeVulkan();
	void FinalizeWindow();

	VkInstance instance_;
	VkDebugUtilsMessengerEXT debugMessenger_;
	constexpr static char APP_NAME[] = "Vulkan Application";

	static void createInstance(VkInstance *dest);
	static bool checkValidationLayerSupport(const std::vector<const char*> &validationLayers);

	static void initializeDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);
	static VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo();

	static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);

	static void finalizeDebugMessenger(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger);

public:
	MyApplication();
	~MyApplication();

	void run() {
		// 初期化
		InitializeWindow();
		InitializeVulkan();

		// 通常処理
		MainLoop();

		// 後片付け
		FinalizeVulkan();
		FinalizeWindow();
	}
};

static std::vector<const char*> getRequiredExtensions()
{
	// 拡張の個数を検出
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);// ★ 追加
	}

#ifdef _DEBUG
	// 有効なエクステンションの表示
	std::cout << "available extensions:" << std::endl;
	for (const auto& extension : extensions) {
		std::cout << "\t" << extension << std::endl;
	}
#endif

	return extensions;
}