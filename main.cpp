#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// function prototypes

void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

unsigned int loadTexture(char const* filename);
unsigned int loadCubeTexture(std::vector<std::string> faces);

// global constants

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 12.0f, -23.6f)); // camera initial position
bool firstMouse = true;
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPosition(0.0f, 10.0f, 0.0f);

float bumpScale = 0.1;

// input flags

bool skyboxOn = false;
bool zPressed = false;
bool lightOn = true;
bool lPressed = false;
bool Blinn = true;
bool bPressed = false;
bool fogOn = false;
bool fPressed = false;
bool monochromeOn = false;
bool mPressed = false;
bool parallaxOn = false;
bool pPressed = true;

int main()
{
    //initialization

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CompGraph", NULL, NULL);
    if (window == NULL) {
        std::cerr << "ERROR: GLFW window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: GLAD initialization failed" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // loading shaders

    Shader commonShader("shaders/common.vs", "shaders/common.fs");
    Shader lightShader("shaders/light.vs", "shaders/light.fs");
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    Shader posteffectShader("shaders/screen.vs", "shaders/screen.fs");
    Shader wallNormalShader("shaders/wallNormal.vs", "shaders/wallNormal.fs");

    // object vertices

    float groundVertices[] = {
        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
         10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f
    };

    float boxVertices[] = {
        // back
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        // front
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        // left
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        // right
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        // bottom
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        // top
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
    };
    glm::vec3 boxPositions[] = {
        glm::vec3(0.0f,  1.2f,  0.0f),
        glm::vec3(-3.2f,  5.5f, 4.3f),
        glm::vec3(6.1f, 2.7f, 2.4f),
        glm::vec3(7.6f, 3.9f, -5.8f),
        glm::vec3(-5.9f, 4.4f, -3.3f)
    };

    float lightVertices[] = {
        // back
        -1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // front
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // left
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        // right
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        // bottom
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        // top
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f , 1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
    };

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float screenVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // calculating vertices for normal mapping

    glm::vec3 pos1(-1.0f, 1.0f, 0.0f), pos2(-1.0f, -1.0f, 0.0f), pos3(1.0f, -1.0f, 0.0f), pos4(1.0f, 1.0f, 0.0f);
    glm::vec3 normal(0.0f, 0.0f, 1.0f);
    glm::vec2 tex1(0.0f, 1.0f), tex2(0.0f, 0.0f), tex3(1.0f, 0.0f), tex4(1.0f, 1.0f);
    glm::vec3 tang1, tang2, bitang1, bitang2;
    
    glm::vec3 edge1 = pos2 - pos1, edge2 = pos3 - pos1;
    glm::vec2 delta1 = tex2 - tex1, delta2 = tex3 - tex1;
    float frac = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);

    tang1.x = frac * (delta2.y * edge1.x - delta1.y * edge2.x);
    tang1.y = frac * (delta2.y * edge1.y - delta1.y * edge2.y);
    tang1.z = frac * (delta2.y * edge1.z - delta1.y * edge2.z);
    bitang1.x = frac * (-delta2.x * edge1.x + delta1.x * edge2.x);
    bitang1.y = frac * (-delta2.x * edge1.y + delta1.x * edge2.y);
    bitang1.z = frac * (-delta2.x * edge1.z + delta1.x * edge2.z);

    edge1 = pos3 - pos1, edge2 = pos4 - pos1;
    delta1 = tex3 - tex1, delta2 = tex4 - tex1;
    frac = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);

    tang2.x = frac * (delta2.y * edge1.x - delta1.y * edge2.x);
    tang2.y = frac * (delta2.y * edge1.y - delta1.y * edge2.y);
    tang2.z = frac * (delta2.y * edge1.z - delta1.y * edge2.z);
    bitang2.x = frac * (-delta2.x * edge1.x + delta1.x * edge2.x);
    bitang2.y = frac * (-delta2.x * edge1.y + delta1.x * edge2.y);
    bitang2.z = frac * (-delta2.x * edge1.z + delta1.x * edge2.z);

    float wallVertices[] = {
        pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, tex1.x, tex1.y, tang1.x, tang1.y, tang1.z, bitang1.x, bitang1.y, bitang1.z,
        pos2.x, pos2.y, pos2.z, normal.x, normal.y, normal.z, tex2.x, tex2.y, tang1.x, tang1.y, tang1.z, bitang1.x, bitang1.y, bitang1.z,
        pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, tex3.x, tex3.y, tang1.x, tang1.y, tang1.z, bitang1.x, bitang1.y, bitang1.z,

        pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, tex1.x, tex1.y, tang2.x, tang2.y, tang2.z, bitang2.x, bitang2.y, bitang2.z,
        pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, tex3.x, tex3.y, tang2.x, tang2.y, tang2.z, bitang2.x, bitang2.y, bitang2.z,
        pos4.x, pos4.y, pos4.z, normal.x, normal.y, normal.z, tex4.x, tex4.y, tang2.x, tang2.y, tang2.z, bitang2.x, bitang2.y, bitang2.z
    };
    glm::vec3 wallPosition (-7.0f, 5.0f, 2.0f);

    // generating vertex arrays and buffers

    // ground
    unsigned int groundVAO, groundVBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // boxes
    unsigned int boxVAO, boxVBO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // wall quad
    unsigned int wallVAO, wallVBO;
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glBindVertexArray(wallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), &wallVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

    // light source
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glBindVertexArray(0);

    // skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glBindVertexArray(0);

    // screen quad (for monochrome mode)
    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    // framebuffer (for monochrome mode)
    unsigned int frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: framebuffer is not complete" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // loading textures

    unsigned int groundTex = loadTexture("textures/Cement.jpg");

    unsigned int boxTextures[5];
    boxTextures[0] = loadTexture("textures/granite.jpg");
    boxTextures[1] = loadTexture("textures/bricks.jpg");
    boxTextures[2] = loadTexture("textures/stone.jpg");
    boxTextures[3] = loadTexture("textures/wood.png");
    boxTextures[4] = loadTexture("textures/yellowstone.jpg");

    unsigned int wallDiffuse = loadTexture("textures/wall_diffuse.jpg");
    unsigned int wallNormal = loadTexture("textures/wall_normal.jpg");
    unsigned int wallBump = loadTexture("textures/wall_bump.jpg");

    std::vector<std::string> skyboxFaces{
        "textures/posx.jpg", "textures/negx.jpg",
        "textures/posy.jpg", "textures/negy.jpg",
        "textures/posz.jpg", "textures/negz.jpg"
    };
    unsigned int skyTex = loadCubeTexture(skyboxFaces);

    // setting uniforms

    commonShader.use();
    commonShader.setInt("tex", 0);
    commonShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    commonShader.setFloat("fogDensity", 0.1f);
    commonShader.setFloat("fogGradient", 0.9f);
    commonShader.setVec3("fogColor", 0.1f, 0.1f, 0.1f);

    wallNormalShader.use();
    wallNormalShader.setInt("diffuseMap", 0);
    wallNormalShader.setInt("normalMap", 1);
    wallNormalShader.setInt("bumpMap", 2);
    wallNormalShader.setFloat("bumpScale", bumpScale);
    wallNormalShader.setFloat("fogDensity", 0.1f);
    wallNormalShader.setFloat("fogGradient", 0.9f);
    wallNormalShader.setVec3("fogColor", 0.1f, 0.1f, 0.1f);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    posteffectShader.use();
    posteffectShader.setInt("scrTexture", 0);

    // print controls to console

    std::cout << "CONTROLS:\n\n";
    std::cout << "WASD - camera movement, mouse - camera rotation, mousewheel - zoom in/out\n";
    std::cout << "Z - toggle skybox (off by default)\n";
    std::cout << "L - toggle lighting (on by default)\n";
    std::cout << "B - switch the lighting between Blinn-Phong model and Phong model (Blinn-Phong model is set by default)\n";
    std::cout << "F - toggle fog (off by default). Fog can be switched on only if skybox is switched off\n";
    std::cout << "M - toggle monochrome mode (off by default)\n";
    std::cout << "P - switch between simple normal mapping and parallax mapping (simple normal mapping is set by default)\n\n";

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glEnable(GL_DEPTH_TEST);
        if (monochromeOn)
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);

        // setting uniforms

        commonShader.use();
        commonShader.setBool("lightOn", lightOn);
        commonShader.setBool("Blinn", Blinn);
        commonShader.setBool("fogOn", fogOn);
        commonShader.setVec3("lightPosition", lightPosition);
        commonShader.setVec3("viewPosition", camera.Position);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        commonShader.setMat4("view", view);
        commonShader.setMat4("projection", projection);

        // rendering ground

        glBindVertexArray(groundVAO);
        glm::mat4 groundModel = glm::mat4(1.0f);
        commonShader.setMat4("model", groundModel);
        commonShader.setFloat("shininess", 2.0);
        glBindTexture(GL_TEXTURE_2D, groundTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // rendering boxes

        glBindVertexArray(boxVAO);
        for (unsigned int i = 0; i < 5; i++) {
            glm::mat4 boxModel = glm::mat4(1.0f);
            boxModel = glm::translate(boxModel, boxPositions[i]);
            if (i == 0) {
                boxModel = glm::rotate(boxModel, 0.25f * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
                boxModel = glm::scale(boxModel, glm::vec3(1.25));
                commonShader.setFloat("shininess", 25.0);
            }
            else if (i == 1) {
                boxModel = glm::rotate(boxModel, 0.5f * (float)glfwGetTime(), glm::vec3(3.4f, 1.1f, 2.8f));
                boxModel = glm::scale(boxModel, glm::vec3(0.5f));
                commonShader.setFloat("shininess", 10.0);
            }
            else if (i == 2) {
                boxModel = glm::rotate(boxModel, 0.75f * (float)glfwGetTime(), glm::vec3(-4.1f, 2.5f, -1.7f));
                boxModel = glm::scale(boxModel, glm::vec3(0.75f));
                commonShader.setFloat("shininess", 20.0);
            }
            else if (i == 3) {
                boxModel = glm::rotate(boxModel, 1.25f * (float)glfwGetTime(), glm::vec3(-2.0f, 1.5f, 4.5f));
                boxModel = glm::scale(boxModel, glm::vec3(1.1f));
                commonShader.setFloat("shininess", 15.0);
            }
            else if (i == 4) {
                boxModel = glm::rotate(boxModel, (float)glfwGetTime(), glm::vec3(1.4f, 3.3f, -3.6f));
                boxModel = glm::scale(boxModel, glm::vec3(0.9f));
                commonShader.setFloat("shininess", 10.0);
            }
            commonShader.setMat4("model", boxModel);

            glBindTexture(GL_TEXTURE_2D, boxTextures[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // rendering wall quad with normal mapping

        wallNormalShader.use();
        wallNormalShader.setMat4("view", view);
        wallNormalShader.setMat4("projection", projection);
        wallNormalShader.setBool("lightOn", lightOn);
        wallNormalShader.setBool("Blinn", Blinn);
        wallNormalShader.setBool("fogOn", fogOn);
        wallNormalShader.setBool("parallaxOn", parallaxOn);
        wallNormalShader.setVec3("viewPosition", camera.Position);
        wallNormalShader.setVec3("lightPosition", lightPosition);
        glm::mat4 wallModel = glm::mat4(1.0f);
        wallModel = glm::translate(wallModel, wallPosition);
        wallModel = glm::rotate(wallModel, -0.1f * (float)glfwGetTime(), glm::vec3(3.0f, 1.0f, 2.0f));
        wallModel = glm::scale(wallModel, glm::vec3(2.5f));
        wallNormalShader.setMat4("model", wallModel);
        wallNormalShader.setFloat("shininess", 15.0);
        glBindVertexArray(wallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wallNormal);
        if (parallaxOn) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, wallBump);
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

        // rendering light source if light is on

        if (lightOn) {
            lightShader.use();
            lightShader.setMat4("view", view);
            lightShader.setMat4("projection", projection);
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, lightPosition);
            lightModel = glm::scale(lightModel, glm::vec3(0.1f));
            lightShader.setMat4("model", lightModel);
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }

        // rendering skybox

        if (skyboxOn) {
            glDepthFunc(GL_LEQUAL);

            skyboxShader.use();
            view = glm::mat4(glm::mat3(camera.getViewMatrix()));
            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyTex);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            glDepthFunc(GL_LESS);
        }

        // monochrome (grayscale) mode

        if (monochromeOn) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            posteffectShader.use();
            glBindVertexArray(screenVAO);
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // memory cleaning

    glDeleteVertexArrays(1, &groundVAO);
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteVertexArrays(1, &wallVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &screenVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &boxVBO);
    glDeleteBuffers(1, &wallVBO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &screenVBO);

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(CameraMovement::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !zPressed) {
        skyboxOn = !skyboxOn;
        zPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
        zPressed = false;

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lPressed) {
        lightOn = !lightOn;
        lPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
        lPressed = false;

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
        Blinn = !Blinn;
        if (Blinn)
            std::cout << "Switched to Blinn-Phong\n";
        else
            std::cout << "Switched to Phong\n";
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
        bPressed = false;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !skyboxOn && !fPressed) {
        fogOn = !fogOn;
        fPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
        fPressed = false;

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mPressed) {
        monochromeOn = !monochromeOn;
        mPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
        mPressed = false;

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed) {
        parallaxOn = !parallaxOn;
        if (parallaxOn)
            std::cout << "Parallax mapping on\n";
        else
            std::cout << "Simple normal mapping on\n";
        pPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
        pPressed = false;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(char const* filename)
{
    unsigned int tex;
    int width, height, channelsNum;

    glGenTextures(1, &tex);
    unsigned char* data = stbi_load(filename, &width, &height, &channelsNum, 0);
    if (data) {
        GLenum internalFormat = GL_RGB, dataFormat = GL_RGB;
        if (channelsNum == 1) {
            internalFormat = GL_RED;
            dataFormat = GL_RED;
        }
        else if (channelsNum == 3) {
            internalFormat = GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (channelsNum == 4) {
            internalFormat = GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        std::cerr << "ERROR: unable to load texture from file " << filename << std::endl;

    stbi_image_free(data);

    return tex;
}

unsigned int loadCubeTexture(std::vector<std::string> faces)
{
    unsigned int tex;
    int width, height, channelsNum;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channelsNum, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cerr << "ERROR: unable to load cubemap from file " << faces[i] << std::endl;
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return tex;
}