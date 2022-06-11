
#include "VulkanBaseApp.h"

int main()
{
	RenderingEngineApplication::VulkanApplication app;

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}