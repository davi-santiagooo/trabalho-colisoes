/* std Libs */
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

/* External Libs */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Internal Includes */
#include "physics.hpp"


namespace fs = std::filesystem;


static float camRadius = 20.0f;
static int heightWin = 200;
static int widthWin = 200;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    heightWin = height;
    widthWin = width;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    extern float camRadius;
    camRadius -= (float)yoffset;
    
    if (camRadius < 2.0f)
        camRadius = 2.0f;
    if (camRadius > 50.0f)
        camRadius = 50.0f;
}


int main(int argc, char* argv[]) {
    
    fs::path exePath = fs::weakly_canonical(fs::path(argv[0])).parent_path().parent_path();
    
    /* #region INIT */
    /* Initilialize GLFW window */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(widthWin, heightWin, "Colisoes3D", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetScrollCallback(window, scroll_callback);
    
    /* Connecting GL to GLFW*/
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    int major = GLAD_VERSION_MAJOR(version);
    int minor = GLAD_VERSION_MINOR(version);
    
    printf("Loaded OpenGL version %d.%d\n", major, minor);
    
    glViewport(0, 0, widthWin, heightWin);
    /* #endregion */
    

    /* #region SHADER LOADING */
    GLint success;
    
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
    
    
    /* Settings Loading */
    fs::path settingsPath = exePath / "settings.txt";
    std::ifstream settingsFile(settingsPath.c_str());
    if(!settingsFile.is_open()) {
        std::cout << "Failed to open settings file" << std::endl;
        return -1;
    }
    
    std::vector<float> arguments;
    std::string varName;
    float value;
    while(settingsFile >> varName >> value) {
        std::cout << value << std::endl;
        arguments.push_back(value);
    }
    std::cout << arguments.size() << std::endl;
    
    int ballNum = (int)arguments[0];
    float poolSize = arguments[1];
    float max_vel0 = arguments[2];
    float restCoeff = arguments[3];
    float physicsdt = arguments[4];
    float mouseSense = arguments[5];
    
    settingsFile.close();
    

    /* Ball initialization */
    std::vector<Ball> balls(ballNum);
    srand(time(NULL));
    float maxVelXYZ = std::sqrt(max_vel0*max_vel0/3);
    {
        int i;
        float posMod;
        float posAdd;
        for(i = 0; i < ballNum; i++) {
            balls[i].radius = 0.5f;

            posMod = poolSize - 2*balls[i].radius;
            posAdd = -(poolSize/2 - balls[i].radius);
            balls[i].pos.x = std::fmod(rand(), posMod) + posAdd;
            balls[i].pos.y = std::fmod(rand(), posMod) + posAdd;
            balls[i].pos.z = std::fmod(rand(), posMod) + posAdd;

            balls[i].vel.x = std::fmod(rand(), maxVelXYZ*2) - maxVelXYZ;
            balls[i].vel.y = std::fmod(rand(), maxVelXYZ*2) - maxVelXYZ;
            balls[i].vel.z = std::fmod(rand(), maxVelXYZ*2) - maxVelXYZ;
        }
    }

    /* Edges */
    float lineCoord = poolSize/2;
    std::vector<float> lines {
        -lineCoord, -lineCoord, -lineCoord,
        lineCoord, -lineCoord, -lineCoord,
        -lineCoord, -lineCoord, -lineCoord,
        -lineCoord, lineCoord, -lineCoord,
        lineCoord, lineCoord, -lineCoord,
        -lineCoord, lineCoord, -lineCoord,
        lineCoord, lineCoord, -lineCoord,
        lineCoord, -lineCoord, -lineCoord,

        -lineCoord, -lineCoord, lineCoord,
        lineCoord, -lineCoord, lineCoord,
        -lineCoord, -lineCoord, lineCoord,
        -lineCoord, lineCoord, lineCoord,
        lineCoord, lineCoord, lineCoord,
        -lineCoord, lineCoord, lineCoord,
        lineCoord, lineCoord, lineCoord,
        lineCoord, -lineCoord, lineCoord,

        -lineCoord, -lineCoord, -lineCoord,
        -lineCoord, -lineCoord, lineCoord,
        lineCoord, -lineCoord, -lineCoord,
        lineCoord, -lineCoord, lineCoord,
        -lineCoord, lineCoord, -lineCoord,
        -lineCoord, lineCoord, lineCoord,
        lineCoord, lineCoord, -lineCoord,
        lineCoord, lineCoord, lineCoord,
    };


    /* Camera */
    glm::vec3 camPos(0.0, 0.0, camRadius);
    glm::vec3 camTarget(0.0);
    glm::vec3 up(0.0, 1.0, 0.0);
    
    /* Sphere Model Loading */
    std::vector<float> sphere;
    std::vector<int> spherei;
    std::vector<float> sphere_normals;
    col_CreateUvSphere(20, 20, 0.5, sphere, spherei, sphere_normals);
    
    /* Matrices */
    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 projection = glm::mat4(1.0);
    glm::mat4 view = glm::mat4(1.0);
    

    /* #region BUFFERS */
    /* Sphere */
    GLuint sphere_vao;
    glGenVertexArrays(1, &sphere_vao);
    glBindVertexArray(sphere_vao);
    
    GLuint vertex_vbo;
    glGenBuffers(1, &vertex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.size() * sizeof(float), sphere.data(), GL_STATIC_DRAW);
    GLint positionAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(positionAttrib);
    
    GLuint normals_vbo;
    glGenBuffers(1, &normals_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere_normals.size() * sizeof(float), sphere_normals.data(), GL_STATIC_DRAW);
    GLint normalsAttrib = glGetAttribLocation(shaderProgram, "aNormal");
    glVertexAttribPointer(normalsAttrib, 3, GL_FLOAT, GL_TRUE, 0, NULL);
    glEnableVertexAttribArray(normalsAttrib);
    
    GLuint indices_ebo;
    glGenBuffers(1, &indices_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, spherei.size() * sizeof(GLuint), spherei.data(), GL_STATIC_DRAW);
    
    /* Lines */
    GLuint line_vao;
    glGenVertexArrays(1, &line_vao);
    glBindVertexArray(line_vao);
    GLuint line_vbo;
    glGenBuffers(1, &line_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(float), lines.data(), GL_STATIC_DRAW);
    positionAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(positionAttrib);
    /* #endregion */
    

    
    /* Uniforms */
    glBindVertexArray(sphere_vao);
    GLuint uni_model = glGetUniformLocation(shaderProgram, "model");
    GLuint uni_view = glGetUniformLocation(shaderProgram, "view");
    GLuint uni_projection = glGetUniformLocation(shaderProgram, "projection");
    
    glEnable(GL_DEPTH_TEST);
    
    /* Main application loop */
    double xpos, ypos;
    double xpos0, ypos0;
    double dxpos, dypos;
    float time, time0, dtime, acumulator;
    glfwGetCursorPos(window, &xpos0, &ypos0);
    time0 = glfwGetTime();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    float camRadiusPrime = camRadius;
    float camAngleh = M_PI / 2;
    float camAnglev = 0.0f;
    while(!glfwWindowShouldClose(window)) {
        
        /* Start */
        glfwGetCursorPos(window, &xpos, &ypos);
        time = glfwGetTime();
        projection = glm::perspective(glm::radians(55.0f), (float)widthWin/heightWin, 0.1f, 100.0f);
        glUniformMatrix4fv(uni_projection, 1, GL_FALSE, glm::value_ptr(projection));
        
        
        /* #region INPUT */
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    
        dxpos = xpos - xpos0;
        dypos = ypos - ypos0;
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
            camAngleh += dxpos * mouseSense;
            camAngleh = std::fmod(camAngleh, M_PI*2);
            
            camAnglev += dypos * mouseSense;
            camAnglev = glm::clamp(camAnglev, (float)(-M_PI/2 + 0.1), (float)(M_PI/2 - 0.1));
        }
        camRadiusPrime = camRadius * std::cos(camAnglev);
        camPos.x = std::cos(camAngleh) * camRadiusPrime;
        camPos.z = std::sin(camAngleh) * camRadiusPrime;
        camPos.y = std::sin(camAnglev) * camRadius;
        /* #endregion */
        
        
        /* Processing */
        view = glm::lookAt(camPos, camTarget, up);
        dtime = time - time0;
        acumulator += dtime;
        
        
        /* #region COLISION */
        int i, j;
        while(acumulator >= physicsdt) {
            for(i = 0; i < ballNum; i++) {
                for(j = i+1; j < ballNum; j++) {
                    glm::vec3 norm = balls[i].pos - balls[j].pos;
                    float dist = glm::length(norm);
                    float radiusSum = balls[i].radius + balls[j].radius;
                    
                    if(dist <= radiusSum) {
                        norm = glm::normalize(norm);
                        float penetration = radiusSum - dist;
                        
                        balls[i].pos += norm * (penetration/2.0f);
                        balls[j].pos -= norm * (penetration/2.0f);
                        
                        glm::vec3 relVel = balls[i].vel - balls[j].vel;
                        float normalVel = glm::dot(relVel, norm);
                        
                        float imp = -(1.0 + restCoeff) * normalVel / 2;

                        if(normalVel < 0.0f) {
                            glm::vec3 impulse = imp * norm;
                            balls[i].vel += impulse;
                            balls[j].vel -= impulse;
                        }
                    }
                }
            }
                
            for(i = 0; i < ballNum; i++) {   
                /* Velocity Update */
                balls[i].pos += physicsdt * balls[i].vel;
                
                /* Wall Collision */
                float wall = poolSize/2 - balls[i].radius;
                if(balls[i].pos.x > wall) {
                    balls[i].pos.x = wall;
                    balls[i].vel.x = -balls[i].vel.x;
                }
                else if (balls[i].pos.x < -wall) {
                    balls[i].pos.x = -wall;
                    balls[i].vel.x = -balls[i].vel.x;
                }

                if(balls[i].pos.y > wall) {
                    balls[i].pos.y = wall;
                    balls[i].vel.y = -balls[i].vel.y;
                }
                else if (balls[i].pos.y < -wall) {
                    balls[i].pos.y = -wall;
                    balls[i].vel.y = -balls[i].vel.y;
                }
                
                if(balls[i].pos.z > wall) {
                    balls[i].pos.z = wall;
                    balls[i].vel.z = -balls[i].vel.z;
                }
                else if (balls[i].pos.z < -wall) {
                    balls[i].pos.z = -wall;
                    balls[i].vel.z = -balls[i].vel.z;
                }
            }

            /* Time Handling */
            acumulator -= physicsdt;
        }
        /* #endregion */
        
        
        /* Draw */
        glUniformMatrix4fv(uni_view, 1, GL_FALSE, glm::value_ptr(view));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        /* Balls */
        for(i = 0; i < ballNum; i++) {
            glUseProgram(shaderProgram);
            glBindVertexArray(sphere_vao);
            
            model = glm::translate(model, glm::vec3(balls[i].pos.x, balls[i].pos[1], balls[i].pos[2]));
            
            glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, spherei.size(), GL_UNSIGNED_INT, (void*)0);
            model = glm::mat4(1.0f);
        }
        glUniformMatrix4fv(uni_model, 1, GL_FALSE, glm::value_ptr(model));
        
        /* Lines */
        glBindVertexArray(line_vao);
        glLineWidth(1.5f);
        glDrawArrays(GL_LINES, 0, 24);
        
        


        /* End */
        xpos0 = xpos;
        ypos0 = ypos;
        time0 = time;
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    /* Free memory and end */
    glfwTerminate();
    return EXIT_SUCCESS;
}


