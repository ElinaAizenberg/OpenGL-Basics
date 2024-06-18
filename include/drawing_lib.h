#ifndef PROJECT_1_DRAWING_LIB_H
#define PROJECT_1_DRAWING_LIB_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <set>

#include "../include/session.h"

class DrawingLib
{
public:
    explicit DrawingLib(Session& session) : session_(session){};

    GLFWwindow* createWindow() const;
    void getWindowSize(GLFWwindow* window);
    void defineCallbackFunction(GLFWwindow* window);

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    void scrollCallback(GLFWwindow* window, double yoffset);

    void drawFrame();
    void drawScene(GLFWwindow* window, bool imGuiCaptureMouse);
    void drawFrameBox() const;
    void drawObjectsMetadata();

private:


    Session& session_;

    int window_width_{1920};
    int window_height_{1080};

    double left_{-1};
    double right_{1};
    double bottom_{-1};
    double top_{1};
    double near_{1};
    double far_{50};

    float dim_ratio_ = static_cast<float>(window_height_) / static_cast<float>(window_width_);

    bool frame_box_{false};
    bool get_color_{false};
    bool left_button_down_{false};
    bool right_button_down_{false};

    double cursor_pos_x_{0}, cursor_pos_y_{0};
    double current_pos_x_{0}, current_pos_y_{0}, prev_pos_x_{0}, prev_pos_y_{0};
    double start_pos_x_{0}, start_pos_y_{0};

    std::vector<int> selected_object_id_{};

    double depth_correction_factor_{8.0f};

    double last_click_time_{0.0};
    const double DOUBLE_CLICK_TIME{0.25}; // 250 ms
    bool imgui_capture_mouse_{false};
    bool left_double_click_{false};

    std::tuple<double, double> calculateCoordinatesOnMouseMove() const;
    void zoom(double zooming_factor);

    std::set<std::array<unsigned char, 3>> getColorsInSelection(int startX, int startY, int endX, int endY) const;
};

#endif //PROJECT_1_DRAWING_LIB_H
