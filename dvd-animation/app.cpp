// Computer Graphics End Of Semester Course Work
// Name: KOKONU, Michael
// Index Number: 3032220


#include <cstdlib>
#include <glm/glm/gtc/quaternion.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <string>
#define WIDTH 800
#define HEIGHT 640
#define TITLE "DVD ANIMATION"


#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"


void processInputs(GLFWwindow *window);
std::string readFile(const std::string &filepath);
void actions(GLFWwindow *window, GLuint shaderProgram, GLuint baseColorLoc);

glm::mat4 trans = glm::mat4(1.0f);
glm::vec3 getColor();


const GLfloat vertices [] =
{
    -0.125f, -1.0f, 0.0f, 0.0f, 1.0f , // bottom-left
    0.125f, -1.0f, 0.0f, 1.0f, 1.0f, // bottom-right
    0.125f, -0.75f, 0.0f, 1.0f, 0.0f, // top-right

    -0.125f, -0.75f, 0.0f, 0.0f, 0.0f, // top-left
};
                        



auto readVertexShaderSource = readFile("../vertex.vs");
const GLchar *vertexShaderSource = readVertexShaderSource.c_str();

auto readFragmentShaderSource = readFile("../fragment.fs");
const GLchar *fragmentShaderSource = readFragmentShaderSource.c_str();




GLfloat curr_time, last_time, delta_time;
GLboolean isPaused = false;
GLchar direction = 'r';


const GLfloat maxX = 1.12f;
const GLfloat minX = -0.87f;
const GLfloat maxY = 2.0f;
const GLfloat minY = 0.002f;
const GLfloat speed = 0.5f;




int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);

    if(window == nullptr)
    {
        std::cout <<"Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    gladLoadGL();


    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<< "Failed to initialse GLAD" << std::endl;
        return -1;
    }

    glViewport(0,0,WIDTH,HEIGHT);

    GLint success;
    char infoLog[512];

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5 * sizeof(float), (void*)( 3* sizeof(float)));

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);


    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout  << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);


    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout  << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

     
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);



    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }



    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("../images/dvd-128.png", &width, &height, &nrChannels, 0);


    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        std::cerr <<"Failed to load image: " << stbi_failure_reason() << std::endl;
    }

    stbi_image_free(data);

   
    glUseProgram(shaderProgram);
   

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    GLuint baseColorLoc = glGetUniformLocation(shaderProgram, "baseColor");

    glm::vec3 colorValue = getColor();
    glUniform3f(baseColorLoc, colorValue.x, colorValue.y, colorValue.z);
    

while(!glfwWindowShouldClose(window))
{
    processInputs(window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    
    curr_time = glfwGetTime();
    delta_time = curr_time - last_time;
    last_time = curr_time;

    actions(window, shaderProgram, baseColorLoc);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));


    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwPollEvents();
    glfwSwapBuffers(window);
}

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
glDeleteProgram(shaderProgram);

glfwTerminate();
return 0;


// Inputs processing
}

void processInputs(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        isPaused = !isPaused;
}


std::string readFile(const std::string &filepath)
{
    std::string fileContents;
    std::ifstream file;
    std::stringstream ss;

    file.open(filepath);
    if(!file.is_open())
    {
        std::cout << "Failed to load file: " << filepath << std::endl;
    }

    ss << file.rdbuf();
    fileContents = ss.str();

    file.close();
    return fileContents;
}



// logic for transition of the object
void actions(GLFWwindow *window, GLuint shaderProgram, GLuint baseColorLoc)
{
    glUseProgram(shaderProgram);

    if (!isPaused)
    {
    
        if (direction == 'r')
        {
            trans = glm::translate(trans, glm::vec3(speed, speed, 0.0f) * delta_time);
           
            if ((trans[3][0] + 0.25) >= maxX)
            {
                direction = 't'; 
                
                glm::vec3 colorValue = getColor();
                glUniform3f(baseColorLoc, colorValue.x, colorValue.y, colorValue.z);

            }
        }
        else if (direction == 't')
        {
            trans = glm::translate(trans, glm::vec3(-speed, speed, 0.0f) * delta_time);

           
            if ((trans[3][1] + 0.2) >= maxY)
            {
                direction = 'l'; 
                glm::vec3 colorValue = getColor();
                glUniform3f(baseColorLoc, colorValue.x, colorValue.y, colorValue.z);
            }
        }
        else if (direction == 'l')
        {
            trans = glm::translate(trans, glm::vec3(-speed, -speed, 0.0f) * delta_time);

            if (trans[3][0] <= minX)
            {
                direction = 'd'; 
                glm::vec3 colorValue = getColor();
                glUniform3f(baseColorLoc, colorValue.x, colorValue.y, colorValue.z);
            }
        }
        else if (direction == 'd')
        {
            trans = glm::translate(trans, glm::vec3(speed, -speed, 0.0f) * delta_time);

            if (trans[3][1] <= minY)
            {
                direction = 'r'; 
                glm::vec3 colorValue = getColor();
                glUniform3f(baseColorLoc, colorValue.x, colorValue.y, colorValue.z);
            }
        }

        std::cout << "Position: (" << trans[3][0] << ", " << trans[3][1] << "), Direction: " << direction << std::endl;
    }
}


//color generation
glm::vec3 getColor(){
    float r = static_cast<float>(rand()) /  static_cast<float>(RAND_MAX);
    float g = static_cast<float>(rand()) /  static_cast<float>(RAND_MAX);
    float b = static_cast<float>(rand()) /  static_cast<float>(RAND_MAX);
    return glm::vec3(r,g,b);
}


