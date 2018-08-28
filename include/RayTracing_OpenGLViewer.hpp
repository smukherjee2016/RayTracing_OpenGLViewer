#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec2 xyPosition;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
" xyPosition = vec2((aPos.x + 1.0)/2.0, (aPos.y + 1.0)/2.0);\n"
"//vertexColor = vec4(1.0 - (aPos.x + 1.0)/2.0,1.0 - (aPos.y + 1.0)/2.0, 0.0, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 xyPosition;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"//if(xyPosition.x < 0.6 && xyPosition.x > 0.4)\n"
"   FragColor = texture(ourTexture, xyPosition);\n"
"}\n\0";

class Shader
{
public:
	// the program ID
	unsigned int ID;

	void createProgram(const char* vShaderCode, const char* fShaderCode)
	{
		// 2. compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		// vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use()
	{
		glUseProgram(ID);
	}

	private:
		// utility function for checking shader compilation/linking errors.
		// ------------------------------------------------------------------------
		void checkCompileErrors(unsigned int shader, std::string type)
		{
			int success;
			char infoLog[1024];
			if (type != "PROGRAM")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}
};

static Shader ourShader;
unsigned int texture;

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

	std::vector<glm::vec3> inputPixels;
    const int WIDTH = 1280;
    const int HEIGHT = 720;
	unsigned int VBO, VAO;

    GLFWwindow* window{};
    
    void createBaseTriangleAndTexture() {

        int width, height;
        glfwGetWindowSize(window, &width, &height);

		float oglVertices[] = {
		-4.0f, -4.0f, 0.0f,
		4.0f, -4.0f, 0.0f,
		0.0f,  4.0f, 0.0f
		};

		ourShader.createProgram(vertexShaderSource, fragmentShaderSource);

		
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

		// We can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);

		//Texture generation
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 256, 256, 0, GL_RGB, GL_FLOAT, inputPixels.data());

    }

    

    static void keyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
        if(action == GLFW_PRESS || action == GLFW_REPEAT) {
            switch (key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                    break;
                
                default:
                    break;
            }
        }

    }

    static void onWindowResized (GLFWwindow* window, const int width, const int height) {
        if (width == 0 || height == 0) return;

        auto* app = reinterpret_cast<RayTracingOpenGLViewer*>(glfwGetWindowUserPointer(window));
        app->resizeView(width, height);

    }

    void initWindow()
    {
		if (!glfwInit()) {
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

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::runtime_error("Failed to initialize GLAD");
		}

        createBaseTriangleAndTexture();
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, RayTracingOpenGLViewer::onWindowResized);
    }
	
    void mainLoop() {
        glfwSetKeyCallback(window, keyCallback);
        while (!glfwWindowShouldClose(window)) {
		
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			glBindTexture(GL_TEXTURE_2D, texture);
			ourShader.use();
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwWaitEvents();
			glfwSwapBuffers(window);

        }

    }

    void cleanup() {

        glfwDestroyWindow(window);

        glfwTerminate();

    }

    void resizeView(int width, int height) {
		glViewport(0, 0, width, height);
    }

};