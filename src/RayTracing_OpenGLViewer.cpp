#include "RayTracing_OpenGLViewer.hpp"

RayTracingOpenGLViewer* RayTracingOpenGLViewer::s_instance = nullptr;



int main() {
	rng.seed(std::random_device()());
    RayTracingOpenGLViewer* app = RayTracingOpenGLViewer::getInstance();
	
	std::vector<glm::vec3> inputPixels;
	for (int i = 0; i <= 255; i++) {
		for (int j = 0; j <= 255; j++) {
			inputPixels.emplace_back(glm::vec3((float)i / 255, (float)j / 255, 0));
		}
	}
	app->setImage(inputPixels);
	
	

    try {

        app->run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}