#include <algorithm>
#include <tuple>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "../include/drawing_lib.h"
#include "../include/config.h"


GLFWwindow* DrawingLib::createWindow() const
/** Creates and returns a new GLFW window with the specified width, height, and title. */
{
    return glfwCreateWindow(window_width_, window_height_, "OpenGL Project 1", nullptr, nullptr);
}

void DrawingLib::getWindowSize(GLFWwindow* window)
/** Retrieves the size of the specified GLFW window and updates the class variables for width, height, and dimension ratio. */
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    window_width_  = w;
    window_height_ = h;
    dim_ratio_ = static_cast<float>(window_height_) / static_cast<float>(window_width_);
}

void DrawingLib::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
/** Handles mouse button events in a GLFW window. If the cursor position is not on any of ImGui elements,
it performs actions on left-click, double left-click and right-click. */
{
    // this boolean is initialized in main.py and checks if mouse position is on any of ImGui elements
    if (!imgui_capture_mouse_)
    {
        double currentTime = glfwGetTime();
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            // get_color_ boolean enables part of the code that reads pixels in cursor position and defines if you interact with Objects
            get_color_ = true;
            if (currentTime - last_click_time_ < DOUBLE_CLICK_TIME) {
                // Reset lastClickTime to avoid detecting triple clicks as double clicks
                last_click_time_ = 0.0;
                left_double_click_ = true;
            }
            else
            {
                last_click_time_ = currentTime;
                left_button_down_ = true;
                glfwGetCursorPos(window, &cursor_pos_x_, &cursor_pos_y_);
                // Objects can be selected with 3 different actions: checkmark in main panel, with drawing selection rectangle, click on the Object itself.
                // Every single click all objects need to be checked if they are selected.
                selected_object_id_ = session_.getSelectedObjects();
            }
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            get_color_ = true;
            right_button_down_ = true;
            glfwGetCursorPos(window, &cursor_pos_x_, &cursor_pos_y_);
            // The same reasoning as above: if several Objects are selected, actions associated with right-click are applied to all Objects.
            selected_object_id_ = session_.getSelectedObjects();
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            left_button_down_   = false;
            if (frame_box_)
            {
                // get_color_ is enabled only for single frame.
                // When you draw a rectangle to select Objects, get_color_ is true the first frame to detect that cursor is on empty area,
                // and the last frame of drawing - to read all pixels inside rectangle.
                get_color_ = true;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            right_button_down_ = false;
            selected_object_id_.clear();
        }
    }
}

void DrawingLib::cursorPositionCallback(GLFWwindow* window,
                                        double input_cursor_pos_x,
                                        double input_cursor_pos_y)
/** Handles cursor movement events in a GLFW window.*/
{
    prev_pos_x_    = current_pos_x_;
    prev_pos_y_    = current_pos_y_;
    current_pos_x_ = input_cursor_pos_x;
    current_pos_y_ = input_cursor_pos_y;

    // if any Objects are selected and left button is down -> move selected Objects
    if (left_button_down_ && !selected_object_id_.empty())
    {
        auto delta_coordinates = calculateCoordinatesOnMouseMove();
        session_.updateObjectsCoordinates(selected_object_id_, std::get<0>(delta_coordinates), std::get<1>(delta_coordinates));

        // if Settings parameter to lock individual gui panels to Objects is  true -> move panels of selected Objects
        if (Config::getParameters().lock_gui_to_objects){
            session_.updateObjectsGuiCoordinates(selected_object_id_, window_width_, window_height_, current_pos_x_ - prev_pos_x_, current_pos_y_ - prev_pos_y_);
        }
    }
    // if any Objects are selected and right button is down -> rotate selected Objects
    if (right_button_down_ && !selected_object_id_.empty())
    {
        double delta_x = (current_pos_x_ - prev_pos_x_);
        double delta_y = (current_pos_y_ - prev_pos_y_);
        // if change in cursor position along x-axis is greater than along y-axis, than rotate along y-axis. And vice versa.
        if (std::abs(delta_x) >= std::abs(delta_y)) {delta_y = 0;}
        else {delta_x = 0;}

        session_.updateObjectsRotation(selected_object_id_, delta_x, delta_y);
    }
    // if left button is down and cursor is not on any Object, starts drawing rectangle to select Objects
    if (left_button_down_ && selected_object_id_.empty())
    {
        // if it's the first frame of drawing rectangle, saves starting coordinates of the rectangle.
        if (!frame_box_)
        {
            frame_box_ = true;
            start_pos_x_ = current_pos_x_;
            start_pos_y_ = current_pos_y_;
        }
    }
}

void DrawingLib::scrollCallback(GLFWwindow* window, double yoffset)
/** Callback function that handles scroll input from the mouse wheel to zoom in or out of the scene.
If ImGui is not capturing the mouse input and the zoom level is adjusted based on the scroll direction. */
{
    // This boolean is created and initialized in main.py. It checks if mouse position is on any of ImGui elements.
    if (!imgui_capture_mouse_)
    {
        if (yoffset > 0)
        {
            zoom(0.1);   // zoom-in
        }
        else if (yoffset < 0)
        {
            zoom(-0.1);   // zoom-out
        }
    }
}

void DrawingLib::zoom(double zooming_factor)
/** Adjusts the viewing boundaries of the scene to achieve zooming in or out.
Modifies the left, right, top, and bottom boundaries of the viewing frustum based on the given zooming factor. */
{
    left_ += zooming_factor;
    right_ -= zooming_factor;
    top_    = top_ - zooming_factor;
    bottom_ = bottom_ + zooming_factor;
}

void DrawingLib::defineCallbackFunction(GLFWwindow* window)
/** Sets up callback functions for a GLFW window.*/
{
    // Sets a custom user-defined pointer (current instance of DrawingLib) to be associated with the GLFW window.
    // It allows the instance's methods to be easily accessed within the GLFW callbacks, enabling event handling
    // (mouse clicks, cursor movements, and scrolling) to interact with the DrawingLib instance.
    glfwSetWindowUserPointer(window, this);

    // These callbacks enable interaction with the window using the mouse for actions such as clicking, dragging, and scrolling.
    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->mouseButtonCallback(win, button, action, mods);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->cursorPositionCallback(win, xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->scrollCallback(win, yoffset);
    });
}

void DrawingLib::drawScene(GLFWwindow* window, bool imGuiCaptureMouse)
/**  Manages the rendering pipeline and interaction handling for the graphical scene using OpenGL and ImGui. */
{
    imgui_capture_mouse_ = imGuiCaptureMouse;

    // Viewport is the region of the window where the rendered image is displayed.
    //It's specified in screen coordinates, with (0, 0) being the bottom-left corner of the window
    glViewport(0, 0, (GLsizei)window_width_, (GLsizei) window_height_);

    //  Enables depth testing, which ensures that objects are rendered in the correct order based on their distance from the camera.
    glEnable(GL_DEPTH_TEST);
    // Fragment passes the depth test if its depth value is less than or equal to the stored depth value.
    glDepthFunc(GL_LEQUAL);
    // Clears the color and depth buffers to preset values, preparing the frame buffer for new rendering.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draws metadata text if 'Show metadata' is selected in Settings
    if (Config::getParameters().show_metadata)
    {
        drawObjectsMetadata();
    }
    // get_color_ is set to true when there is an interaction with GLFW window.
    if (get_color_)
    {
        double x_coord, y_coord;
        unsigned char color[3];
        // When get_color_ is true, Objects are drawn with pick_colors (not general colors), and every Object has a unique pick_color,
        // that allows to identify Object id.
        drawFrame();
        glfwGetCursorPos(window, &x_coord, &y_coord);

        glReadPixels(x_coord, window_height_ - y_coord, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
        auto object_id = session_.getObjectIdByPickColor(color);

        if (left_double_click_)
        {
            // if double left-click is on empty area and there are selected Objects, all Objects are deselected.
            if (!selected_object_id_.empty() && object_id < 0)
            {
                session_.deSelectAllObjects();
                selected_object_id_.clear();
            }
            // if double left-click is on a Object, opens individual ImGui window
            if (object_id >= 0)
            {
                session_.getObjects()[object_id].setGuiWindowCoordinates(window_width_, window_height_, cursor_pos_x_, cursor_pos_y_);
                session_.getObjects()[object_id].switchGuiEnabled();
            }
            left_double_click_ = false;
        }
        else
        {
            // get_color_ is set to true when frame_box_ and left button is released that indicates end of rectangle drawing
            if (frame_box_)
            {
                selected_object_id_.clear();
                // reads pixels inside the drawn rectangle to get a set of unique colors
                auto colors = getColorsInSelection(start_pos_x_, start_pos_y_, current_pos_x_, current_pos_y_);

                for (auto c : colors)
                {
                    // same steps to identify selected Objects as when there is left-clicking on the Object
                    auto obj_id = session_.getObjectIdByPickColor(&c[0]);
                    if (obj_id >= 0 && std::find(selected_object_id_.begin(), selected_object_id_.end(), obj_id) == selected_object_id_.end()) {
                        selected_object_id_.push_back(obj_id);
                    }
                }
                frame_box_ = false;
                session_.selectObjectsInFrame(selected_object_id_);
            }
            // if left- ot right-click on an Object, its id is added the vector selected_object_id_ and
            // following manipulations to Objects are applied to all Objects in this vetor
            if (object_id >= 0 && std::find(selected_object_id_.begin(), selected_object_id_.end(), object_id) == selected_object_id_.end())
            {
                selected_object_id_.push_back(object_id);
            }
        }

    }
    else
    {
        drawFrame(); // draw frame with regular colours
    }

    if (frame_box_)
    {
        drawFrameBox();
    }

    ImGui::Render(); // Finalizes the ImGui frame and prepares the draw data for rendering.
    // Renders the compiled ImGui draw data using the OpenGL 3 backend.
    // Takes the draw data and issues the necessary OpenGL commands to display the ImGui interface.
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (!get_color_)
    {
        // Swaps the front and back buffers of the specified window.
        // In double-buffered mode, rendering is done to the back buffer while the front buffer is displayed on the screen.
        // Buffers should be swapped only when Objects are drawn with regular colors (not pick_colors).
        glfwSwapBuffers(window);
    }
    get_color_ = false;
}

void DrawingLib::drawFrame()
/** Sets up the projection and model-view matrices for a perspective view, then translates the scene and draws all objects.*/
{
    // Switches the current matrix mode to the projection matrix.
    // It indicates that subsequent matrix operations (like glLoadIdentity(), glOrtho(), glFrustum(), etc.)
    // will affect the projection matrix.
    glMatrixMode(GL_PROJECTION);

    // Resets the transformation to its initial state, meaning no transformations are applied.
    // This is typically done before you apply any custom transformations to objects in your scene
    glLoadIdentity();

    // Defines a perspective matrix that produces a perspective projection.
    // This projection matrix transforms coordinates from 3D world space to 2D screen space.
    // The resulting view frustum is a truncated pyramid.
    glFrustum(left_, right_, bottom_ * dim_ratio_, top_ * dim_ratio_, near_, far_);

    // After setting up the projection matrix, switches to GL_MODELVIEW mode to handle model transformations.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -depth_correction_factor_);

    session_.drawAllObjects(get_color_);
}

std::tuple<double, double> DrawingLib::calculateCoordinatesOnMouseMove() const
/** Calculates the change in object coordinates of mouse cursor, converting screen space coordinates to
 normalized device coordinates (NDC) and then to frustum coordinates, with depth correction applied. */
{
    // Convert the mouse movement (delta_x and delta_y) from screen space to normalized device coordinates (NDC)
    // where the range is [-1, 1].
    double ndc_delta_x = (current_pos_x_ - prev_pos_x_) / (window_width_ / 2.0f);
    double ndc_delta_y = (current_pos_y_ - prev_pos_y_) / (window_height_ / 2.0f);

    // Compute the width and height of the frustum at the near plane
    double frustum_width = right_ - left_;
    double frustum_height = (top_ * dim_ratio_) - (bottom_ * dim_ratio_);

    // Scale normalized deltas to match the frustum coordinates
    double adjusted_delta_x = ndc_delta_x * frustum_width / 2.0f;
    double adjusted_delta_y = ndc_delta_y * frustum_height / 2.0f;

    // Apply depth correction factor (assuming linear scaling for simplicity)
    adjusted_delta_x *= depth_correction_factor_;
    adjusted_delta_y *= depth_correction_factor_;

    std::tuple<double, double> delta_coordinates = std::make_tuple(adjusted_delta_x, adjusted_delta_y);

    return delta_coordinates;
}

void DrawingLib::drawFrameBox() const
/** Draws a rectangular selection box on the screen. */
{
    glMatrixMode( GL_PROJECTION ); // switches the current matrix mode to the projection matrix.
    glLoadIdentity();

    // Sets the projection matrix to orthographic mode aligned with the window dimensions.
    glOrtho( 0, window_width_, window_height_, 0, -1, 1 );

    // After setting up the projection matrix, switchs to GL_MODELVIEW mode to handle model transformations.
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Sets the polygon mode to draw lines.
    glColor3f(1,1,0); // Sets the colour to yellow for the frame box.

    // Draws a rectangle with the coordinates of the starting and current mouse positions.
    glBegin(GL_QUADS);
    glVertex2f(start_pos_x_,start_pos_y_);
    glVertex2f(current_pos_x_,start_pos_y_);
    glVertex2f(current_pos_x_,current_pos_y_);
    glVertex2f(start_pos_x_,current_pos_y_);
    glEnd();
}


void DrawingLib::drawObjectsMetadata()
/** Draws metadata for all objects in the scene. */
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Sets the projection matrix to orthographic mode based on the viewing boundaries scaled by
    // a factor of depth correction factor that is used in drawing all Objects.
    glOrtho(left_*depth_correction_factor_, right_*depth_correction_factor_,
            bottom_ * dim_ratio_*depth_correction_factor_, top_ * dim_ratio_*depth_correction_factor_,
            -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Sets the background color to black.
    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    // Sets the alignment requirement to 1 byte for the start of each pixel row in memory.
    // By setting the alignment to 1, OpenGL is aware that pixel rows can start at any byte boundary.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glColor3f(1,1,1);  // Sets the colour to white.
    session_.drawAllObjectsMetadata();
}


std::set<std::array<unsigned char, 3>> DrawingLib::getColorsInSelection(int startX, int startY, int endX, int endY) const
/** Identifies and returns unique colors within a specified rectangular area of the screen. */
{
    int width = abs(endX - startX);
    int height = abs(endY - startY);

    startX = std::min(startX, endX); // to ensure start represents the left side of the rectangle.
    startY = std::max(startY, endY); // to ensure it represents the top side of the rectangle

    // If the width or height is zero, there's nothing to read
    if (width == 0 || height == 0) {
        return {};
    }

    // Ensure the pixel vector is correctly sized.
    // By default, the alignment for read operations GL_PACK_ALIGNMENT is set to 4 bytes.
    std::vector<unsigned char> pixels(width * height * 4);

    // Read pixel data from the frame buffer
    glReadPixels(startX, window_height_-startY, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Set to store unique colors
    std::set<std::array<unsigned char, 3>> uniqueColors;

    // Process the pixel data to identify unique colors
    for (int i = 0; i < width * height * 4; i += 4)
    {
        std::array<unsigned char, 3> color = { pixels[i], pixels[i + 1], pixels[i + 2] };
        uniqueColors.insert(color);
    }

    return uniqueColors;
}
