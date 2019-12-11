#include "MyApplication.h"

MyApplication::MyApplication()
{
}


MyApplication::~MyApplication()
{
}

//初期化
void MyApplication::InitializeWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// OpenGL の種類の設定
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// ユーザーはウィンドウサイズを変更できない

	window_ = glfwCreateWindow(WIDTH, HEIGHT, APP_NAME, nullptr, nullptr);
}
void MyApplication::InitializeVulkan() {

	createInstance(&instance_);
	initializeDebugMessenger(instance_, debugMessenger_);
}

//通常処理
void MyApplication::MainLoop() {

	while (!glfwWindowShouldClose(window_))
	{
		glfwPollEvents();
	}

}

//片付け
void MyApplication::FinalizeVulkan() {
	finalizeDebugMessenger(instance_, debugMessenger_);
	vkDestroyInstance(instance_, nullptr);
}

void MyApplication::FinalizeWindow() {

	glfwDestroyWindow(window_);
	glfwTerminate();

}

void MyApplication::createInstance(VkInstance *dest) {

	// アプケーション情報を定めるための構造体
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;    // 構造体の種類
	appInfo.pApplicationName = APP_NAME;                   // アプリケーション名
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // 開発者が決めるバージョン番号
	appInfo.pEngineName = "My Engine";                     // ゲームエンジン名
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);      // ゲームエンジンのバージョン
	appInfo.apiVersion = VK_API_VERSION_1_0;               // 使用するAPIのバージョン

	// 新しく作られるインスタンスの設定の構造体
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // 構造体の種類
	createInfo.pApplicationInfo = &appInfo;                    // VkApplicationInfoの情報

	// valkanの拡張機能を取得して、初期化データに追加
	std::vector<const char*> extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {// ★このif文と中を追加
		static const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"

		};

		// 検証レイヤーの確認
		if (!checkValidationLayerSupport(validationLayers)) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// インスタンスへの設定
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		// デバッグメッセンジャーもその後に引き続いて作成する
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = populateDebugMessengerCreateInfo();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}

	// インスタンスの生成
	if (vkCreateInstance(&createInfo, nullptr, dest) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

// 検証レイヤーに対応しているか確認
bool MyApplication::checkValidationLayerSupport(const std::vector<const char*> &validationLayers)
{
	// レイヤーのプロパティを取得
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);               // レイヤー数の取得
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());// プロパティ自体の取得

	// 全てのレイヤーが検証レイヤーに対応しているか確認
	for (const char* layerName : validationLayers) {
		if (![](const char* layerName, const auto &availableLayers) {
			// レイヤーが検証レイヤーのどれかを持っているか確認
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) { return true; }
			}
			return false;
		}(layerName, availableLayers)) {
			return false;
		}// どこかのレイヤーがvalidationLayersのレイヤーに対応していないのはダメ
	}

	return true;
}

void MyApplication::initializeDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger)
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = populateDebugMessengerCreateInfo();// 生成情報の構築

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {// 生成
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkDebugUtilsMessengerCreateInfoEXT MyApplication::populateDebugMessengerCreateInfo() {
	
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |  // 診断メッセージ
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |                          // リソースの作成などの情報メッセージ(かなり表示される)
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |                       // 必ずしもエラーではないが、アプリケーションのバグである可能性が高い動作に関するメッセージ
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;                          // 無効であり、クラッシュを引き起こす可能性のある動作に関するメッセージ
	
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |          // 仕様またはパフォーマンスとは無関係のイベントが発生
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |                        // 仕様に違反する、または間違いの可能性を示すものが発生
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;                        // Vulkanの最適でない使用の可能性

	createInfo.pfnUserCallback = [](
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) -> VKAPI_ATTR VkBool32
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	};

	return createInfo;
}

VkResult MyApplication::CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	// vkCreateDebugUtilsMessengerEXTに対応しているか確認して実行
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func == nullptr) return VK_ERROR_EXTENSION_NOT_PRESENT;
	return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}


void MyApplication::finalizeDebugMessenger(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger)
{
	if (!enableValidationLayers) return;

	// vkCreateDebugUtilsMessengerEXTに対応しているか確認して実行
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func == nullptr) return;
	func(instance, debugMessenger, nullptr);
}
