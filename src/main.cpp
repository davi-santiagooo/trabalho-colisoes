/* std Libs */
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

/* External Libs */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

/* Internal Includes */
#include "sphere.hpp"

namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}


int main(int argc, char* argv[]) {
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
    fs::path exePath = fs::weakly_canonical(fs::path(argv[0])).parent_path().parent_path();

    /* Vertex shader */
    fs::path vertexPath = exePath / "assets" / "vertex.glsl";
    std::ifstream vertexFile(vertexPath.c_str());
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
    fs::path fragmentPath = exePath / "assets" / "fragment.glsl";
    std::ifstream fragmentFile(fragmentPath.c_str());
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


    glm::vec3 cam_pos(0.0, 0.0, 0.0);
    glm::vec3 cam_dir(0.0, 0.0, 1.0);

    std::vector<float> sphere;
    std::vector<int> spherei;
    std::vector<float> sphere_normals;
    col_CreateUvSphere(20, 20, 0.5, sphere, spherei, sphere_normals);

    /* Matrices */
    glm::mat4 model = glm::mat4(1.0);

    glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)width/height, 0.1f, 100.0f);

    glm::mat4 view = glm::mat4(1.0);
    view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));

    /* VAOs */
    GLuint sphere_vao;
    glGenVertexArrays(1, &sphere_vao);
    glBindVertexArray(sphere_vao);
    
    /* VBOs */
    GLuint vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.size() * sizeof(GLfloat), sphere.data(), GL_STATIC_DRAW);
    
    /* EBOs */
    GLuint indices_vbo;
    glGenBuffers(1, &indices_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, spherei.size() * sizeof(GLuint), spherei.data(), GL_STATIC_DRAW);
    
    /* Vertex Attributes */
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    /* Uniforms */
    GLuint uni_objColor = glGetUniformLocation(shaderProgram, "objColor");
    GLuint uni_model = glGetUniformLocation(shaderProgram, "model");
    GLuint uni_view = glGetUniformLocation(shaderProgram, "view");
    GLuint uni_projection = glGetUniformLocation(shaderProgram, "projection");

    glEnable(GL_DEPTH_TEST);

    /* Main application loop */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    while(!glfwWindowShouldClose(window)) {
        /* Start */
        processInput(window);
        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0, 0.0, 0.0));


        /* Draw */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform3f(uni_objColor, 1.0, 0.0, 0.0);
        
        glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uni_projection, 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(shaderProgram);
        glBindVertexArray(sphere_vao);
        glDrawElements(GL_TRIANGLES, spherei.size(), GL_UNSIGNED_INT, (void*)0);

        /* End */
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    /* Free memory and end */
    glfwTerminate();
    return 0;
}


