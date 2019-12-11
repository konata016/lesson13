#include "MyApplication.h"

MyApplication::MyApplication()
{
}


MyApplication::~MyApplication()
{
}

//������
void MyApplication::InitializeWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);	// OpenGL �̎�ނ̐ݒ�
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// ���[�U�[�̓E�B���h�E�T�C�Y��ύX�ł��Ȃ�

	window_ = glfwCreateWindow(WIDTH, HEIGHT, APP_NAME, nullptr, nullptr);
}
void MyApplication::InitializeVulkan() {

	createInstance(&instance_);
	initializeDebugMessenger(instance_, debugMessenger_);
}

//�ʏ폈��
void MyApplication::MainLoop() {

	while (!glfwWindowShouldClose(window_))
	{
		glfwPollEvents();
	}

}

//�Еt��
void MyApplication::FinalizeVulkan() {
	finalizeDebugMessenger(instance_, debugMessenger_);
	vkDestroyInstance(instance_, nullptr);
}

void MyApplication::FinalizeWindow() {

	glfwDestroyWindow(window_);
	glfwTerminate();

}

void MyApplication::createInstance(VkInstance *dest) {

	// �A�v�P�[�V���������߂邽�߂̍\����
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;    // �\���̂̎��
	appInfo.pApplicationName = APP_NAME;                   // �A�v���P�[�V������
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // �J���҂����߂�o�[�W�����ԍ�
	appInfo.pEngineName = "My Engine";                     // �Q�[���G���W����
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);      // �Q�[���G���W���̃o�[�W����
	appInfo.apiVersion = VK_API_VERSION_1_0;               // �g�p����API�̃o�[�W����

	// �V���������C���X�^���X�̐ݒ�̍\����
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // �\���̂̎��
	createInfo.pApplicationInfo = &appInfo;                    // VkApplicationInfo�̏��

	// valkan�̊g���@�\���擾���āA�������f�[�^�ɒǉ�
	std::vector<const char*> extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {// ������if���ƒ���ǉ�
		static const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"

		};

		// ���؃��C���[�̊m�F
		if (!checkValidationLayerSupport(validationLayers)) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// �C���X�^���X�ւ̐ݒ�
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		// �f�o�b�O���b�Z���W���[�����̌�Ɉ��������č쐬����
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = populateDebugMessengerCreateInfo();
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)& debugCreateInfo;
	}

	// �C���X�^���X�̐���
	if (vkCreateInstance(&createInfo, nullptr, dest) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

// ���؃��C���[�ɑΉ����Ă��邩�m�F
bool MyApplication::checkValidationLayerSupport(const std::vector<const char*> &validationLayers)
{
	// ���C���[�̃v���p�e�B���擾
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);               // ���C���[���̎擾
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());// �v���p�e�B���̂̎擾

	// �S�Ẵ��C���[�����؃��C���[�ɑΉ����Ă��邩�m�F
	for (const char* layerName : validationLayers) {
		if (![](const char* layerName, const auto &availableLayers) {
			// ���C���[�����؃��C���[�̂ǂꂩ�������Ă��邩�m�F
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) { return true; }
			}
			return false;
		}(layerName, availableLayers)) {
			return false;
		}// �ǂ����̃��C���[��validationLayers�̃��C���[�ɑΉ����Ă��Ȃ��̂̓_��
	}

	return true;
}

void MyApplication::initializeDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger)
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = populateDebugMessengerCreateInfo();// �������̍\�z

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {// ����
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkDebugUtilsMessengerCreateInfoEXT MyApplication::populateDebugMessengerCreateInfo() {
	
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |  // �f�f���b�Z�[�W
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |                          // ���\�[�X�̍쐬�Ȃǂ̏�񃁃b�Z�[�W(���Ȃ�\�������)
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |                       // �K�������G���[�ł͂Ȃ����A�A�v���P�[�V�����̃o�O�ł���\������������Ɋւ��郁�b�Z�[�W
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;                          // �����ł���A�N���b�V���������N�����\���̂��铮��Ɋւ��郁�b�Z�[�W
	
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |          // �d�l�܂��̓p�t�H�[�}���X�Ƃ͖��֌W�̃C�x���g������
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |                        // �d�l�Ɉᔽ����A�܂��͊ԈႢ�̉\�����������̂�����
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;                        // Vulkan�̍œK�łȂ��g�p�̉\��

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
	// vkCreateDebugUtilsMessengerEXT�ɑΉ����Ă��邩�m�F���Ď��s
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func == nullptr) return VK_ERROR_EXTENSION_NOT_PRESENT;
	return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}


void MyApplication::finalizeDebugMessenger(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger)
{
	if (!enableValidationLayers) return;

	// vkCreateDebugUtilsMessengerEXT�ɑΉ����Ă��邩�m�F���Ď��s
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func == nullptr) return;
	func(instance, debugMessenger, nullptr);
}
