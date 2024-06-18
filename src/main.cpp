#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "logger.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../include/session.h"
#include "../include/drawing_lib.h"
#include "../include/gui_panels.h"
#include "../include/config.h"


Parameters Config::parameters_;
ImGuiAl::Log* Logger::log_panel_;
bool Logger::p_open_{true};
char Logger::log_buffer_[kBufferSize];



int main()
{
    Logger::init();
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Session
    Session session = Session();
    DrawingLib drawing_lib    = DrawingLib(session);
    GuiPanels gui_panels = GuiPanels(session);
    Logger::addMessage(LogLevel::Info, "Welcome to OpenGL examples: project_1!");

    GLFWwindow* window = drawing_lib.createWindow();
    glfwMakeContextCurrent(window);
    drawing_lib.defineCallbackFunction(window);

    GLenum res = glewInit();
    if (res)
    {
        std::cout << glewGetErrorString(res) << std::endl;
    }

    if (res != GLEW_OK)
    {
        std::terminate();
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGui::StyleColorsDark();

    session.add_object(kIcosahedron);

    ImGui_ImplOpenGL3_CreateFontsTexture();
    while (glfwWindowShouldClose(window) == 0)
    {

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui_panels.drawMainPanel();
        gui_panels.drawObjectsPanels();
        drawing_lib.getWindowSize(window);

        // Check if ImGui wants to capture the mouse
        bool ioWantCaptureMouse = ImGui::GetIO().WantCaptureMouse;

        drawing_lib.drawScene(window, ioWantCaptureMouse);

        glfwPollEvents();
        GLuint res = glGetError();
        if (res)
        {
            std::cout << glewGetErrorString(res) << std::endl;
        }

    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
