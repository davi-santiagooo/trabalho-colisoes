/* std Libs */
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

/* External Libs */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <vector>

/* Internal Includes */


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}


int main() {
    int height = 200;
    int width = 200;


    /* #region INIT */
    /* Initilialize GLFW window */
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
    
    /* Connecting GL to GLFW*/
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
        
    int major = GLAD_VERSION_MAJOR(version);
    int minor = GLAD_VERSION_MINOR(version);
    
    printf("Loaded OpenGL version %d.%d\n", major, minor);
    
    glViewport(0, 0, width, height);
    /* #endregion */


    /* #region SHADER LOADING */
    GLint success;

    /* Vertex shader */
    std::ifstream vertexFile("assets/vertex.glsl");
    if(!vertexFile.is_open()) {
        std::cout << "Failed to open vertex shader file" << std::endl;
        return -1;
    }

    std::stringstream vShaderStream;
    vShaderStream << vertexFile.rdbuf();
    vertexFile.close();

    std::string vertexCode = vShaderStream.str();
    const char* vertexShaderSource = vertexCode.c_str();
        
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char log_buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, log_buffer);
        int i;
        printf("SHADER::VERTEX::ERROR: ");
        for(i = 0; log_buffer[i] != '\0'; i++) printf("%c", log_buffer[i]);
    }
    
    
    /* Fragment shader */
    std::ifstream fragmentFile("assets/fragment.glsl");
    if(!fragmentFile.is_open()) {
        std::cout << "Failed to open fragment shader file" << std::endl;
        return -1;
    }

    std::stringstream fShaderStream;
    fShaderStream << fragmentFile.rdbuf();
    fragmentFile.close();

    std::string fragmentCode = fShaderStream.str();
    const char* fragmentShaderSource = fragmentCode.c_str();
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char log_buffer[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, log_buffer);
        int i;
        printf("SHADER::FRAGMENT::ERROR: ");
        for(i = 0; log_buffer[i] != '\0'; i++) printf("%c", log_buffer[i]);
    }
    
    /* Create shader program*/
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char log_buffer[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, log_buffer);
        int i;
        printf("SHADER::PROGRAM::ERROR: ");
        for(i = 0; log_buffer[i] != '\0'; i++) printf("%c", log_buffer[i]);
    }
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    /* #endregion */


    std::vector<GLfloat> square = {
        -0.5, -0.5, 0.0,
        -0.5, 0.5,  0.0,
        0.5,  0.5,  0.0,
        0.5,  -0.5,0.0,
    };
    std::vector<GLuint> square_index = {
        0, 1, 2,
        0, 3, 2,
    };

    /* VAOs */
    GLuint square_vao;
    glGenVertexArrays(1, &square_vao);
    glBindVertexArray(square_vao);
    
    /* VBOs */
    GLuint vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(GLfloat), square.data(), GL_STATIC_DRAW);
    
    /* EBOs */
    GLuint indices_vbo;
    glGenBuffers(1, &indices_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, square_index.size() * sizeof(GLuint), square_index.data(), GL_STATIC_DRAW);
    
    /* Vertex Attributes */
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    /* Uniforms */
    GLint uni_objColor = glGetUniformLocation(shaderProgram, "objColor");

    /* Main application loop */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    while(!glfwWindowShouldClose(window)) {
        /* Start */
        processInput(window);

        /* Draw */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform3f(uni_objColor, 1.0, 0.0, 0.0);

        glUseProgram(shaderProgram);
        glBindVertexArray(square_vao);
        glDrawElements(GL_TRIANGLES, square_index.size(), GL_UNSIGNED_INT, (void*)0);

        /* End */
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    /* Free memory and end */
    glfwTerminate();
    return 0;
}


