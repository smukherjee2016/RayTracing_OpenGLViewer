#include "RayTracing_OpenGLViewer.hpp"
#include <chrono>
#include <thread>

RayTracingOpenGLViewer* RayTracingOpenGLViewer::s_instance = nullptr;

static thread_local std::mt19937 rng;
static thread_local std::uniform_real_distribution<double> rando(0.0f, 1.0f);


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

        std::thread t1(&RayTracingOpenGLViewer::run, app);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100000));
		for (int i = 0; i <= 255; i++) {
			for (int j = 0; j <= 255; j++) {
				inputPixels.emplace_back(glm::vec3(rando(rng), rando(rng), rando(rng)));
			}
		}
		app->setImage(inputPixels);
	}

    return EXIT_SUCCESS;
}