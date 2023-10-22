﻿// main.cpp: 定义应用程序的入口点。
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <thread>

#include "stb_image.h"
#include "shader.h"
//#include "data.h"
#include "ortho_camera.h"
//#include "data_producer.h"
#include "data_producer_multi.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
Vertex calVertex(Vertex v1, Vertex v2, float p);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
OrthoCamera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float lef = -1.0f, rig = 1.0f;
float bottom = -0.75f, top = 0.75f;
float near = 0.1f, far = 100.0f;

float leftBorder = -6.4f, rightBorder = 6.4f;
float bottomBorder = -3.2f, topBorder = 3.2f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//int main()
//{
//    Data data;
//    data.readDataFromJson();
//    data.readDataFromJson();
//}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "THE", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("../../../shader/shader.vs", "../../../shader/shader.fs");
    Shader mapShader("../../../shader/map.vs", "../../../shader/map.fs");

    glEnable(GL_PROGRAM_POINT_SIZE);
    stbi_set_flip_vertically_on_load(true);
    float mapVertices[] = {
        // positions          // colors           // texture coords
         6.4f,  3.2f, -1.0f,   1.0f, 1.0f, // top right
         6.4f, -3.2f, -1.0f,   1.0f, 0.0f, // bottom right
        -6.4f, -3.2f, -1.0f,   0.0f, 0.0f, // bottom left
        -6.4f,  3.2f, -1.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    unsigned int VBO[2], VAO[2], EBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);


    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mapVertices), mapVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color  attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* mapData = stbi_load("../../../resources/map.jpg", &width, &height, &nrChannels, 0);
    if (mapData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, mapData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(mapData);


    Data data;
    for (int i = 0; i < 5; i++) {
        data.getDataAsyc();
    }
    

    float oneSec = 0.0f;
    float lastSec = 0.0f;
    float currentSec = 0.0f;
    int fps = 0;
    
    DataChunk currentChunk = data.getDataChunk();
    DataChunk nextChunk = data.getDataChunk();
    //cout << currentChunk.indices.size() << "cur " << currentChunk.vertices.size() << endl;
    //cout << nextChunk.indices.size() << "nex " << nextChunk.vertices.size() << endl;

    float currentDelta = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        fps++;
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        //cout << deltaTime << endl;
        lastFrame = currentFrame;
        


        currentDelta += deltaTime;
        if (currentDelta > 1.0f) {
            currentChunk = nextChunk;
            nextChunk = data.getDataChunk();
            currentDelta = 0.0f;
            cout << "fps: " << fps << endl;
            fps = 0;
        }

        //cout << nextChunk.indices.size() << " " << nextChunk.vertices.size() << endl;

        vector<Vertex> vertices;
        for (int i = 0; i < currentChunk.vertices.size(); i++) {
            vertices.push_back(calVertex(currentChunk.vertices[i], nextChunk.vertices[i], currentDelta));
        }
        vector<int> indexLines = currentDelta < 0.5f ? currentChunk.indices : nextChunk.indices;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glBindTexture(GL_TEXTURE_2D, texture);

        //draw map
        mapShader.use();

        //lef = -1.0f, rig = 1.0f;
        //bottom = -float(SCR_HEIGHT) / float(SCR_WIDTH), top = float(SCR_HEIGHT) / float(SCR_WIDTH);

        /*lef = -1.0f, rig = 1.0f;
        bottom = -float(SCR_HEIGHT) / float(SCR_WIDTH), top = float(SCR_HEIGHT) / float(SCR_WIDTH);*/
        glm::mat4 projection = glm::ortho(lef, rig, bottom, top, near, far);
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        mapShader.setMat4("projection", projection);
        mapShader.setMat4("view", view);
        mapShader.setMat4("model", model);
        
       
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // activate shader
        ourShader.use();

        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        float time1 = static_cast<float>(glfwGetTime());

        
        //draw points
        float time2 = static_cast<float>(glfwGetTime());




        time1 = static_cast<float>(glfwGetTime());



        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indexLines.size(), &indexLines[0], GL_DYNAMIC_DRAW);
   
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // color  attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);
       
        glDrawArrays(GL_POINTS, 0, vertices.size());

        time2 = static_cast<float>(glfwGetTime());
        //cout << "draw points time : " << time2 - time1 << endl;


        glDrawElements(GL_LINES, indexLines.size(), GL_UNSIGNED_INT, 0);

        
        time2 = static_cast<float>(glfwGetTime());
        //cout << "draw lines time : " << time2 - time1 << endl;
   

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, topBorder, top);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, bottomBorder, bottom);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, leftBorder, lef);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, rightBorder, rig);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.

    glViewport(0, 0, width, height);
    cout << "width: " << width << "height: " << height << endl;
    bottom = float(height) / float(width) * lef, top = float(height) / float(width) * rig;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{   
    glm::vec3 Position = camera.getPosition();
    float posX = camera.getAbs(Position.x);
    float posY = camera.getAbs(Position.y);
    //cout << "x : " << xoffset << "y : " << yoffset << endl;
    if (yoffset < 0.f && (posX - yoffset * 0.1f + rig > rightBorder || posY - yoffset * 0.1f + top > topBorder)) {
        return;
    }
    lef += yoffset * 0.1f;
    rig -= yoffset * 0.1f;
    top -= yoffset * 0.1f;
    bottom += yoffset * 0.1f;
    //camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

Vertex calVertex(Vertex v1, Vertex v2, float p) {
    glm::vec3 v = v1.position + (v2.position - v1.position) * p;
    Vertex vertex(v, v1.color);
    return vertex;
}

