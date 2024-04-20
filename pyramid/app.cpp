#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


void processInputs(GLFWwindow *window);

float coordinates [] = {
    //positions                      //color                             // texture
    0.0f, 0.5f,0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 
    -0.5f, -0.5f , -0.5f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
};

GLuint indices[] {
    0,1,2,
    0,3,4,
    0,1,3,
    0,2,4,
    1,2,3,
    2,3,4
};

const char *vertexShaderSource = "#version 330 core\n"
                    "layout (location=0) in vec3 coords;\n"
                    "layout (location=1) in vec3 aColor;\n"
                    "out vec3 ourColor;\n"
                    "uniform mat4 model;\n"
                    "uniform mat4 view;\n"
                    "uniform mat4 projection;\n"
                    "void main(){\n"
                    "gl_Position =  projection * view * model * vec4(coords,1.0);\n"
                    "ourColor = aColor;\n"
                    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                "out vec4 FragColor;\n"
                "in vec3 ourColor;\n"
                "void main(){\n"
                "FragColor = vec4(ourColor, 1.0);\n"
                "}\0";


int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800,600, "Pyramid", nullptr,nullptr);

    if(window == nullptr){
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0,0,800,600);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(float), (void*)( 3* sizeof(float)));



    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout  << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    

    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::rotate(model, glm::radians(-55.0f),
    //                             glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we’re translating the scene in the reverse direction
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));


    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f,100.0f);
    //glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

    glUseProgram(shaderProgram);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window)){
        processInputs(window);
        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        float curr_frame = glfwGetTime();
        float rotation_speed = curr_frame * 45.0f;

         glm::mat4 model = glm::mat4(1.0f);
         model = glm::rotate(model,  glm::radians(rotation_speed),glm::vec3(0.5f, 1.0f, 0.0f));
         glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES,18, GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwTerminate();
    return 0;

}

void processInputs(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}