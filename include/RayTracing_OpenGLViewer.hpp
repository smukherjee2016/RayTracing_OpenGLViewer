#pragma once

#include <iostream>

// Include GLEW. Include it before gl.h and glfw3.h.
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>

static thread_local std::mt19937 rng;
static thread_local std::uniform_real_distribution<double> rando(-1.0f, 1.0f);

class RayTracingOpenGLViewer {

    static RayTracingOpenGLViewer *s_instance;

public:

    static RayTracingOpenGLViewer *getInstance() {
        if (s_instance == nullptr) {
            s_instance = new RayTracingOpenGLViewer;
        }
        return s_instance;
    }

    void run() {
        initWindow();
        initOpenGL();
        mainLoop();
        cleanup();
    }

private:
    //Right-handed coordinate system, same as GL_MODELVIEW
    //http://www.songho.ca/opengl/files/gl_anglestoaxes01.png
    enum moveDirection {
        RIGHT_X_POSITIVE,
        LEFT_X_NEGATIVE,
        UP_Y_POSITIVE,
        DOWN_Y_NEGATIVE,
        FORWARD_Z_NEGATIVE,
        BACKWARD_Z_POSITIVE,
        NONE_NOTHING
    };

    const int WIDTH = 1280;
    const int HEIGHT = 720;

    GLFWwindow* window{};

    // TODO(): Put these in a camera class later
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    const glm::vec3 lookingAtPos = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    const float fOV = glm::radians(45.0f);
    const float near = 0.1f;
    const float far = 10.0f;
    const float defaultAspectRatio = static_cast<float>(WIDTH) / HEIGHT;
    const float moveSensitivity = 0.05f;

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } cameraPosition{};

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    void populateVertices() {

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        //vertices = {
        //	 {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        //	{{0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        //	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        //	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        //};
        vertices = {
                { { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
                { { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
                { { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
                { { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
        };

        indices = {
                0, 1, 2, 2, 3, 0
        };
    }

    void setDefaultCamera() {
        cameraPosition.model = glm::mat4(1.0f);
        cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
        cameraPosition.proj = glm::perspective(fOV, defaultAspectRatio, near, far);
        //cameraPosition.proj[1][1] *= -1; //Readjust for Vulkan

    }

    void updateCameraPosition(moveDirection inputDirection) {
        switch (inputDirection) {

            case LEFT_X_NEGATIVE:
                cameraPos = cameraPos - glm::vec3(moveSensitivity, 0.0f, 0.0f);
                cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
                break;

            case RIGHT_X_POSITIVE:
                cameraPos = cameraPos + glm::vec3(moveSensitivity, 0.0f, 0.0f);
                cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
                break;

            case UP_Y_POSITIVE:
                cameraPos = cameraPos + glm::vec3(0.0f, moveSensitivity, 0.0f);
                cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
                break;

            case DOWN_Y_NEGATIVE:
                cameraPos = cameraPos - glm::vec3(0.0f, moveSensitivity, 0.0f);
                cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
                break;

            case FORWARD_Z_NEGATIVE:
                cameraPos = cameraPos - glm::vec3(0.0f, 0.0f, moveSensitivity);
                cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
                break;

            case BACKWARD_Z_POSITIVE:
                cameraPos = cameraPos + glm::vec3(0.0f, 0.0f, moveSensitivity);
                cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
                break;

            case NONE_NOTHING: //For updating camera position with proper swapChainExtent values
                //cameraPosition.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width /  static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);
                cameraPosition.proj[1][1] *= -1;
                break;
            default:
                break;
        }



    }

    static void keyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
        if(action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;
                case GLFW_KEY_W:
                    getInstance()->updateCameraPosition(FORWARD_Z_NEGATIVE);
                    break;

                case GLFW_KEY_A:
                    getInstance()->updateCameraPosition(LEFT_X_NEGATIVE);
                    break;

                case GLFW_KEY_S:
                    getInstance()->updateCameraPosition(BACKWARD_Z_POSITIVE);
                    break;

                case GLFW_KEY_D:
                    getInstance()->updateCameraPosition(RIGHT_X_POSITIVE);
                    break;

                case GLFW_KEY_Q:
                    getInstance()->updateCameraPosition(UP_Y_POSITIVE);
                    break;

                case GLFW_KEY_E:
                    getInstance()->updateCameraPosition(DOWN_Y_NEGATIVE);
                    break;

                default:
                    break;
            }
        }

    }

    static void onWindowResized (GLFWwindow* window, const int width, const int height) {
        if (width == 0 || height == 0) return;

        auto* app = reinterpret_cast<RayTracingOpenGLViewer*>(glfwGetWindowUserPointer(window));
        app->recreateSwapChain();

    }

    void initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "RayTracing_OpenGLViewer", nullptr, nullptr);

        populateVertices();
        setDefaultCamera();
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, RayTracingOpenGLViewer::onWindowResized);
    }

    void mainLoop() {
        glfwSetKeyCallback(window, keyCallback);
        while (!glfwWindowShouldClose(window)) {
            //glfwWaitEventsTimeout(1.0);
            glfwWaitEvents();
            updateCameraPosition(NONE_NOTHING);
            drawFrame();
        }

    }

    void initOpenGL() {

    }

    void cleanup() {

        glfwDestroyWindow(window);

        glfwTerminate();

    }

    void drawFrame() {

    }

    void recreateSwapChain() {

    }

};