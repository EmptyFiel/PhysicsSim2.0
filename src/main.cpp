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

// Vertex Shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    out vec3 FragPos;
    out vec3 Normal;
    
    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    
    in vec3 FragPos;
    in vec3 Normal;
    
    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 lightColor;
    uniform vec3 objectColor;
    
    void main() {
        // Ambient lighting
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse lighting
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Specular lighting
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;
        
        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
    }
)";

// Global variables for window dimensions and physics
int windowWidth = 800;
int windowHeight = 600;

// Physics variables
struct SpherePhysics {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    float radius;
    float bounceDamping;
    glm::vec3 color;  // Added color property
};

// Initialize sphere physics
SpherePhysics sphere = {
    glm::vec3(0.0f, 0.0f, 0.0f),    // Starting position
    glm::vec3(0.0f, 0.0f, 0.0f),    // Initial velocity
    glm::vec3(0.0f, 0.0f, 0.0f),   // Gravity acceleration
    1.0f,                           // Mass
    1.0f,                           // Radius
    0.8f,                           // Bounce damping factor
    glm::vec3(1.0f, 0.0f, 0.0f)     // color
};

SpherePhysics sphere2 = {
    glm::vec3(3.0f, 2.0f, 0.0f),    // starting position
    glm::vec3(1.0f, 0.0f, 0.0f),   //  initial velocity
    glm::vec3(1.0f, 0.0f, 0.0f),   // Same gravity
    1.0f,                           // mass
    1.0f,                           // Same radius
    0.9f,                           // Different bounce damping
    glm::vec3(0.0f, 0.0f, 1.0f)     //color
};

SpherePhysics sphere3 = {
    glm::vec3(-3.0f, -2.0f, 0.0f),    // starting position
    glm::vec3(0.0f, 0.0f, 0.0f),   //  initial velocity
    glm::vec3(0.0f, 0.0f, 0.0f),   // Same gravity
    1.0f,                           // mass
    1.0f,                           // Same radius
    0.9f,                           // Different bounce damping
    glm::vec3(0.0f, 1.0f, 0.0f)     //color
};

bool playback = true;

// Resize callback function
void resizeWindow(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

// Physics update function
void updatePhysics(SpherePhysics& sphere, const SpherePhysics& sphere1, const SpherePhysics& sphere2, float deltaTime) {
	if (!playback) return;
		// Update velocity based on acceleration
		sphere.velocity += sphere.acceleration * deltaTime;
		
		// Update position based on velocity
		sphere.position += sphere.velocity * deltaTime;
		glm::vec3 change1 = sphere1.position - sphere.position;
		glm::vec3 change2 = sphere2.position - sphere.position;
		glm::vec3 combinedchange = change1 + change2
}

// Add force to sphere (for user interaction)
void addForce(SpherePhysics& sphere, glm::vec3 force) {
    sphere.acceleration += force / sphere.mass;
}

// Function to generate sphere vertices and normals
void generateSphereVertices(int latRes, int lonRes, float radius, float*& vertices, unsigned int*& indices, int& vertexCount, int& indexCount) {
    const float PI = 3.14159265359f;
    
    vertexCount = (latRes + 1) * (lonRes + 1);
    indexCount = latRes * lonRes * 6;
    
    vertices = new float[vertexCount * 6]; // 3 for position, 3 for normal
    indices = new unsigned int[indexCount];
    
    int vertexIndex = 0;
    
    // Generate vertices
    for (int lat = 0; lat <= latRes; lat++) {
        float theta = PI * ((float)lat / (float)latRes);
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        
        for (int lon = 0; lon <= lonRes; lon++) {
            float phi = 2.0f * PI * ((float)lon / (float)lonRes);
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            
            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;
            
            // Position
            vertices[vertexIndex * 6] = radius * x;
            vertices[vertexIndex * 6 + 1] = radius * y;
            vertices[vertexIndex * 6 + 2] = radius * z;
            
            // Normal (same as position for unit sphere)
            vertices[vertexIndex * 6 + 3] = x;
            vertices[vertexIndex * 6 + 4] = y;
            vertices[vertexIndex * 6 + 5] = z;
            
            vertexIndex++;
        }
    }
    
    // Generate indices
    int indexIdx = 0;
    for (int lat = 0; lat < latRes; lat++) {
        for (int lon = 0; lon < lonRes; lon++) {
            int first = lat * (lonRes + 1) + lon;
            int second = first + lonRes + 1;
            
            // First triangle
            indices[indexIdx++] = first;
            indices[indexIdx++] = second;
            indices[indexIdx++] = first + 1;
            
            // Second triangle
            indices[indexIdx++] = second;
            indices[indexIdx++] = second + 1;
            indices[indexIdx++] = first + 1;
        }
    }
}

// Function to check shader compilation errors
void checkShaderCompilation(unsigned int shader, const char* type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR: " << type << " shader compilation failed\n" << infoLog << std::endl;
    }
}

// Function to check shader program linking errors
void checkProgramLinking(unsigned int program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader program linking failed\n" << infoLog << std::endl;
    }
}

// Function to draw a sphere with given properties
void drawSphere(unsigned int shaderProgram, unsigned int VAO, int indexCount, 
                const SpherePhysics& sphere, float currentTime,
                const glm::mat4& view, const glm::mat4& projection,
                const glm::vec3& lightPos, const glm::vec3& cameraPos, 
                const glm::vec3& lightColor,
                int modelLoc, int viewLoc, int projectionLoc, 
                int lightPosLoc, int viewPosLoc, int lightColorLoc, int objectColorLoc) {
    
    // Model matrix (translate to sphere's current position)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, sphere.position);
    // Optional: add rotation for visual effect
    model = glm::rotate(model, currentTime * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f));
    
    // Set uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(objectColorLoc, 1, glm::value_ptr(sphere.color));
    
    // Draw sphere
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Sim", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Set resize callback
    glfwSetFramebufferSizeCallback(window, resizeWindow);
    
    // Keyboard input callback
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            
            switch (key) {
                case GLFW_KEY_SPACE: playback = !playback; break;
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
            }
        }
    });
    
    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Set viewport
    glViewport(0, 0, 800, 600);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Generate sphere vertices
    int latRes = 30;
    int lonRes = 30;
    float radius = 1.0f;
    float* vertices;
    unsigned int* indices;
    int vertexCount, indexCount;
    
    generateSphereVertices(latRes, lonRes, radius, vertices, indices, vertexCount, indexCount);
    
    // Create and bind a Vertex Array Object (VAO)
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 6 * sizeof(float), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Clean up vertex data
    delete[] vertices;
    delete[] indices;
    
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader, "VERTEX");
    
    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader, "FRAGMENT");
    
    // Link shaders into a shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinking(shaderProgram);
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Get uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    
    // Camera settings
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 30.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    // Lighting settings
    glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    
    // Timing variables
    float lastTime = glfwGetTime();
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {	
		// Calculate delta time
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		
		// Update physics for both spheres
		updatePhysics(sphere, sphere2, sphere3, deltaTime);
		updatePhysics(sphere2, sphere, sphere3, deltaTime);
		updatePhysics(sphere3, sphere, sphere2, deltaTime);
		
		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Calculate matrices
		float aspectRatio = (float)windowWidth / (float)windowHeight;
		
		// View matrix
		glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
		
		// Projection matrix (perspective)
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
		
		// Draw first sphere
		drawSphere(shaderProgram, VAO, indexCount, sphere, currentTime, 
				  view, projection, lightPos, cameraPos, lightColor,
				  modelLoc, viewLoc, projectionLoc, lightPosLoc, viewPosLoc, lightColorLoc, objectColorLoc);
		
		// Draw second sphere
		drawSphere(shaderProgram, VAO, indexCount, sphere2, currentTime, 
				  view, projection, lightPos, cameraPos, lightColor,
				  modelLoc, viewLoc, projectionLoc, lightPosLoc, viewPosLoc, lightColorLoc, objectColorLoc);
		
		// Draw third sphere
		drawSphere(shaderProgram, VAO, indexCount, sphere3, currentTime, 
				  view, projection, lightPos, cameraPos, lightColor,
				  modelLoc, viewLoc, projectionLoc, lightPosLoc, viewPosLoc, lightColorLoc, objectColorLoc);
		
		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
    }
    
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    
    return 0;
}