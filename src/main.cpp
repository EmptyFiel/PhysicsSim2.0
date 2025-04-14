#define GLFW_INCLUDE_NONE
#include "../include/GLFW/glfw3.h"
#include "../include/glad/glad.h"
#include "../include/KHR/khrplatform.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <iostream>
#include <cmath>
//g++ ./src/main.cpp -o main -I./include -L./lib -lglad -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

//add this some where glfwSetFramebufferSizeCallback(window, resizeWindow);

// unsigned int shaderProgram;

// void resizeWindow(GLFWwindow* window, int width, int height) {
    // glViewport(0, 0, width, height);
    
    // float aspect = (float)width / (float)height;
    // glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    
    //Update the projection matrix uniform in your shader
    // glUseProgram(shaderProgram);
    // unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    // glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
// }

// Vertex Shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    }
)";

void generateCircleVertices(int res, float radius, float* vertices){
	const float PI = 3.14159265359f;
	
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = 0.0f;
	
	for(int i = 0; i <= res; i++){
		float theta = 2.0f * PI * (float)i / (float)res;
		vertices[(i + 1)*3] = radius * cos(theta);
		vertices[(i + 1)*3 + 1] = radius * sin(theta);
		vertices[(i + 1)*3 + 2] = 0.0f;
	}
};

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Sim", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	
    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Define triangle vertices
    // float vertices[] = {
        // -0.5f, -0.5f, 0.0f, // Left
         // 0.5f, -0.5f, 0.0f, // Right
         // 0.0f,  0.5f, 0.0f  // Top
    // };
	
	int res = 40;
	float radius = 0.5f;
	float* vertices = new float[(res + 2) * 3];
	generateCircleVertices(res, radius, vertices);
	
    // Create and bind a Vertex Array Object (VAO)
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (res + 2) * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	
	delete[] vertices;

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link shaders into a shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	// glfwSetFramebufferSizeCallback(window, resizeWindow);
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, res + 2);
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}




