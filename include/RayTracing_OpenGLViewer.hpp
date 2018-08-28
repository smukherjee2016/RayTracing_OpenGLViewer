#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>

static thread_local std::mt19937 rng;
static thread_local std::uniform_real_distribution<double> rando(-1.0f, 1.0f);

const char * vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char * fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

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
	unsigned int VBO, VAO;
	int shaderProgram;

    GLFWwindow* window{};

    // TODO(): Put these in a camera class later
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    const glm::vec3 lookingAtPos = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    const float fOV = glm::radians(45.0f);
    float nearPoint = 0.1;
    float farPoint = 10.0f;
    const float defaultAspectRatio = static_cast<float>(WIDTH) / HEIGHT;
    const float moveSensitivity = 0.05f;

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } cameraPosition{};

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    std::vector<Vertex> vertices;


    void populateVertices() {

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        vertices = {
        	 {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        	{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        	{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        };
        /*vertices = {
                { { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
                { { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
                { { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
                { { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
        };*/

		float oglVertices[] = {
		-4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 0.0f,
		0.0f,  4.0f, 0.0f
		};

		// build and compile our shader program
	// ------------------------------------
	// vertex shader
		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// fragment shader
		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// check for shader compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);


		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(oglVertices), oglVertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);



    }

    void setDefaultCamera() {
        cameraPosition.model = glm::mat4(1.0f);
        cameraPosition.view = glm::lookAt(cameraPos, lookingAtPos, upVector);
        cameraPosition.proj = glm::perspective(fOV, defaultAspectRatio, nearPoint, farPoint);
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
		if (!glfwInit()) {
			glfwTerminate();
			std::runtime_error("Failed to initialize GLFW!");
		}
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif	
		

        window = glfwCreateWindow(WIDTH, HEIGHT, "RayTracing_OpenGLViewer", nullptr, nullptr);
		glfwMakeContextCurrent(window);

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::runtime_error("Failed to initialize GLAD");
		}

        populateVertices();
        setDefaultCamera();
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, RayTracingOpenGLViewer::onWindowResized);
    }

    void mainLoop() {
        glfwSetKeyCallback(window, keyCallback);
        while (!glfwWindowShouldClose(window)) {
		
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(shaderProgram);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwWaitEvents();
			glfwSwapBuffers(window);
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