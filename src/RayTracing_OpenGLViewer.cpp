#include "RayTracing_OpenGLViewer.hpp"

RayTracingOpenGLViewer* RayTracingOpenGLViewer::s_instance = nullptr;


int main() {

    rng.seed(std::random_device()());

    RayTracingOpenGLViewer* app = RayTracingOpenGLViewer::getInstance();

    try {
        app->run();
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}