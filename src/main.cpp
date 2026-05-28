#include <ios>
#include <iostream>
#include <fstream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int height = 200;
int width = 200;

int main() {

    std::streamsize fileSize;

    /* Read vertex shader file */
    std::ifstream vertexFile("../assets/vertex.glsl", std::ios::ate);
    if(!vertexFile.is_open()) {
        std::cout << "Failed to open vertex shader file" << std::endl;
        return -1;
    }

    fileSize = vertexFile.tellg();
    fileSize++;
    vertexFile.seekg(0, std::ios::beg);
    
    char* vertexBuffer = new char[fileSize + 1];
    vertexFile.read(vertexBuffer, fileSize);
    vertexBuffer[fileSize] = '\0';
    const char* vertexShaderSource = vertexBuffer;
    delete[] vertexBuffer;

    vertexFile.close();
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    
    /* Read fragment shader file */
    std::ifstream fragmentFile("../assets/fragment.glsl", std::ios::ate);
    if(!fragmentFile.is_open()) {
        std::cout << "Failed to open fragment shader file" << std::endl;
        return -1;
    }

    fileSize = fragmentFile.tellg();
    fileSize++;
    fragmentFile.seekg(0, std::ios::beg);
    
    char* fragmentBuffer = new char[fileSize + 1];
    fragmentFile.read(fragmentBuffer, fileSize);
    fragmentBuffer[fileSize] = '\0';
    const char* fragmentShaderSource = fragmentBuffer;
    delete[] fragmentBuffer;

    fragmentFile.close();

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Colisoes3D", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLFW + GL
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
        
    int major = GLAD_VERSION_MAJOR(version);
    int minor = GLAD_VERSION_MINOR(version);
    
    printf("Loaded OpenGL version %d.%d\n", major, minor);

    glViewport(0, 0, width, height);


    while(!glfwWindowShouldClose(window))
    {
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}


