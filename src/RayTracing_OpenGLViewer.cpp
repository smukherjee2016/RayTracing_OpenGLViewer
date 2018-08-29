#include "RayTracing_OpenGLViewer.hpp"

RayTracingOpenGLViewer* RayTracingOpenGLViewer::s_instance = nullptr;
static thread_local std::mt19937 rng;
static thread_local std::uniform_real_distribution<double> rando(0.0f, 1.0f);

//Demonstration of execution from other place
//This function passes the pixels to display to OpenGL
std::vector<glm::vec3> createImage() {
	std::vector<glm::vec3> inputPixels;
	for (int i = 0; i <= 255; i++) {
		for (int j = 0; j <= 255; j++) {
			//inputPixels.emplace_back(glm::vec3((float)i / 255, (float)j / 255, 0));
			inputPixels.emplace_back(glm::vec3(rando(rng), rando(rng), rando(rng)));
		}
	}

	//std::cout << "Hello world";
	return inputPixels;
}

int main() {
	rng.seed(std::random_device()());
    RayTracingOpenGLViewer* app = RayTracingOpenGLViewer::getInstance();
	
	try {
		//C++11
		auto createImageFunctionBind = std::bind(&createImage);
		std::function<std::vector<glm::vec3>()> createImageFunction = createImageFunctionBind;
        app->run(createImageFunction);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}