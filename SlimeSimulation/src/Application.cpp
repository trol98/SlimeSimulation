#include "vendor/imgui/imgui.h"

#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "TextureHelper.h"
#include "Agent.h"
#include "AgentCreator.h"

#include "vendor/glm/glm/glm.hpp"
#include "vendor/glm/glm/gtc/matrix_transform.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, double deltaTime);

//window
constexpr unsigned SCR_WIDTH = 1920, SCR_HEIGHT = 990;
//constexpr unsigned SCR_WIDTH = 800, SCR_HEIGHT = 600;
//constexpr unsigned SCR_WIDTH = 320, SCR_HEIGHT = 180;

float trailWeight = 5.0f;
float decayRate = 0.0001f;
float diffuseRate = 3.0f;
// max number of agents on my computer is 2^16 - 1
constexpr unsigned numAgents = 65535;
constexpr unsigned numSpecies = 3;

//float moveSpeed = 0.0;
//float turnSpeed = 2.0;
//float sensorAngleDegrees = 30.0;
//float sensorOffsetDst = 35.0;
//int   sensorSize = 1;

int main()
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    srand(time(nullptr));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::puts("Failed to initialize OpenGL context");
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");

    Shader simpleShader("SlimeSimulation/res/shaders/simpleVertex.glsl",
                        "SlimeSimulation/res/shaders/simpleFragment.glsl");
    Shader computeShader("SlimeSimulation/res/shaders/slimeCompute.glsl");
    Shader diffComputeShader("SlimeSimulation/res/shaders/diffuseCompute.glsl");
    
    Agent* agents = AgentCreator::createAgents(numAgents, numSpecies, AgentMode::InwardCircle,
        SCR_WIDTH, SCR_HEIGHT);

    SpeciesSettings* settings = new SpeciesSettings[numSpecies];

    settings[0] = {
        14.0f,
        14.0f,
        30.0f,
        24.0f,
        3, 
        1.0f,
        0.0f,
        0.0f
    };
    settings[1] = {
    14.0f,
    2.0f,
    30.0f,
    12.0f,
    2,
    0.0f,
    1.0f,
    0.0f
    };

    settings[2] = {
    6.0f,
    4.0f,
    45.0f,
    36.0f,
    4,
    0.0f,
    0.0f,
    1.0f
    };


    GLuint ssboAgents;
    glGenBuffers(1, &ssboAgents);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboAgents);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numAgents * sizeof(Agent), agents, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboAgents);

    GLuint ssboSpecs;
    glGenBuffers(1, &ssboSpecs);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpecs);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numSpecies * sizeof(SpeciesSettings), settings, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboSpecs);

    unsigned int textureBoard = TextureHelper::loadEmptyTexture(SCR_WIDTH, SCR_HEIGHT, 4);

    float quadData[] =
    {
          // positions       // texture Coords
          1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
          1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    };
    unsigned int quadIndicies[] = {
        0, 1, 3, 
        1, 2, 3
    };

    unsigned int quadVBO, quadEBO, quadVAO;

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndicies), quadIndicies, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // set uniform variables that will never change
    simpleShader.use();
    simpleShader.setInt("textureBoard", 0);

    computeShader.use();
    computeShader.setInt("boardImage", 0);
    computeShader.setFloat("width", SCR_WIDTH);
    computeShader.setFloat("height", SCR_HEIGHT);

    diffComputeShader.use();
    diffComputeShader.setInt("boardImage", 0);
    diffComputeShader.setFloat("width", SCR_WIDTH);
    diffComputeShader.setFloat("height", SCR_HEIGHT);
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        //ImGui::ShowDemoWindow(&show_demo_window);
        {
            ImGui::Begin("Overall settings!");
            ImGui::DragFloat("decayRate", &decayRate,     0.0001f, 0.0f, 0.5f);
            ImGui::DragFloat("diffuseRate", &diffuseRate, 0.01f, 0.0f, 5.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        {
            ImGui::Begin("Species settings");
            for (int i = 0; i < numSpecies; i++)
            {
                auto& sets = settings[i];
                ImGui::Text(("Species nr." + std::to_string(i)).c_str());
                ImGui::DragFloat(("moveSpeed" + std::to_string(i)).c_str(), & (sets.moveSpeed), 1.0f, 0.0f, 100.0f);
                ImGui::DragFloat(("turnSpeed" + std::to_string(i)).c_str(), & (sets.turnSpeed), 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat(("sensorAngleDegrees" + std::to_string(i)).c_str(), & (sets.sensorAngle), 1.0f, 0.0f, 360.0f);
                ImGui::DragFloat(("sensorOffsetDst" + std::to_string(i)).c_str(), & (sets.sensorOffset), 1.0f, 0.0f, 100.0);
                ImGui::DragInt(("sensorSize" + std::to_string(i)).c_str(), &(sets.sensorSize), 1, 0, 100);
                // TODO: Add color choice
            }
            ImGui::End();
        }

        // "specSettings"
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, textureBoard, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        computeShader.use();
        computeShader.setFloat("deltaTime", deltaTime);
        computeShader.setFloat("currentFrame", currentFrame);
        
        for (int i = 0; i < numSpecies; i++)
        {
            std::string name = "specSettings[" + std::to_string(i) + "].";

            computeShader.setFloat(name + "moveSpeed", settings[i].moveSpeed);
            computeShader.setFloat(name + "turnSpeed", settings[i].turnSpeed);
            computeShader.setFloat(name + "sensorAngleDegrees", settings[i].sensorAngle);
            computeShader.setFloat(name + "sensorOffsetDst", settings[i].sensorOffset);
            computeShader.setInt(name + "sensorSize", settings[i].sensorSize);
            computeShader.setFloat(name + "r", settings[i].r);
            computeShader.setFloat(name + "g", settings[i].g);
            computeShader.setFloat(name + "b", settings[i].b);
        }
        
        glDispatchCompute(numAgents, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        diffComputeShader.use();
        diffComputeShader.setFloat("deltaTime", deltaTime);
        diffComputeShader.setFloat("decayRate", decayRate);
        diffComputeShader.setFloat("diffuseRate", diffuseRate);

        glDispatchCompute(SCR_WIDTH, SCR_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        /* Render here */

        simpleShader.use();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
       
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

    }
    delete [] agents;
    delete [] settings;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, double deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
