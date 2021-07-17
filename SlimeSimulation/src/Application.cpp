#include "vendor/imgui/imgui.h"

#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "TextureHelper.h"
#include "AgentCreator.h"

#include "vendor/glm/glm/glm.hpp"
#include "vendor/glm/glm/gtc/matrix_transform.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, double deltaTime);

//window
//constexpr unsigned SCR_WIDTH = 1920, SCR_HEIGHT = 1080;
constexpr unsigned SCR_WIDTH = 800, SCR_HEIGHT = 600;
//constexpr unsigned SCR_WIDTH = 320, SCR_HEIGHT = 180;

float trailWeight = 5.0f;
float decayRate = 0.0001f;
float diffuseRate = 3.0f;
unsigned numAgents = 500;


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

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    Shader simpleShader("SlimeSimulation/res/shaders/simpleVertex.glsl",
        "SlimeSimulation/res/shaders/simpleFragment.glsl");

    Shader computeShader("SlimeSimulation/res/shaders/slimeCompute.glsl");
    Shader diffComputeShader("SlimeSimulation/res/shaders/diffuseCompute.glsl");

    
    Agent* agents = AgentCreator::createRandom(numAgents, SCR_WIDTH, SCR_HEIGHT);


    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numAgents * sizeof(Agent), agents, GL_DYNAMIC_READ); //sizeof(data) only works for statically sized C/C++ arrays.
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

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

    simpleShader.use();
    simpleShader.setInt("textureBoard", 0);

    computeShader.use();
    computeShader.setInt("boardImage", 0);
    computeShader.setFloat("trailWeight", trailWeight);
    computeShader.setFloat("decayRate", decayRate);
    computeShader.setFloat("diffuseRate", diffuseRate);
    computeShader.setFloat("width", SCR_WIDTH);
    computeShader.setFloat("height", SCR_HEIGHT);

    diffComputeShader.use();
    diffComputeShader.setInt("boardImage", 0);
    diffComputeShader.setFloat("width", SCR_WIDTH);
    diffComputeShader.setFloat("height", SCR_HEIGHT);
    diffComputeShader.setFloat("decayRate", decayRate);
    diffComputeShader.setFloat("diffuseRate", diffuseRate);

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
        ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, textureBoard, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        computeShader.use();
        computeShader.setFloat("deltaTime", deltaTime);
        computeShader.setFloat("currentFrame", currentFrame);

        glDispatchCompute(numAgents, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        diffComputeShader.use();
        diffComputeShader.setFloat("deltaTime", deltaTime);

        glDispatchCompute(SCR_WIDTH, SCR_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);
        //glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

        simpleShader.use();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
       
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

    }
    delete agents;

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
