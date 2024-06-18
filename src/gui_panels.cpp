#include <fstream>
#include <sstream>
#include "imgui.h"
#include "logger.h"
#include "imguial_term.h"

#include "../include/gui_panels.h"
#include "../include/config.h"


void GuiPanels::drawMainPanel()
/** Draws the main panel that includes following tabs: Objects, Settings, Logger, Help. */
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 200), ImVec2(800, 600));
    ImGui::Begin("Main panel");

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None))
    {
        drawObjectsTab();
        drawSettingsTab();
        drawLoggerTab();
        drawHelpTab();

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void GuiPanels::drawObjectsTab()
/** Draws a tab Objects that combines several drawing functions. */
{
    if (ImGui::BeginTabItem("Objects"))
    {
        drawCreateObjects();
        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button("Select all")){
            session_.selectAllObjects();
        }
        ImGui::SameLine();
        if (ImGui::Button("De-select all")){
            session_.deSelectAllObjects();
        }
        ImGui::Spacing();
        ImGui::Spacing();

        drawObjectsList();

        ImGui::EndTabItem();
    }
}

void GuiPanels::drawCreateObjects()
/** Draws a combo to select an object type (Cube, Icosahedron, Sphere, Pyramid) from a hard-coded list and a button
to add a new object to the session. Also, it adds an info message to logger with id and type of new object. */
{
    if (ImGui::Button("+"))
    {
        auto it = object_types_.find(current_obj_type_);
        auto selected_type = it->second;
        session_.add_object(selected_type);
    }
    ImGui::SameLine();

    if (ImGui::BeginCombo("object type", current_obj_type_.c_str(), ImGuiComboFlags_None))
    {
        for (const auto& obj_type: object_types_)
        {
            bool is_selected = (obj_type.first == current_obj_type_);
            if (ImGui::Selectable(obj_type.first.c_str(), is_selected))
                current_obj_type_ = obj_type.first;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void GuiPanels::drawObjectsList()
/** Iterates through the vector of objects in the session and draws a collapsing header with individual settings
per object. */
{
    auto& objects = session_.getObjects();

    for (auto object_id = 0; object_id < objects.size(); object_id++){
        auto& object = objects[object_id];
        drawObjectItemInList(object, object_id);
    }
}

void GuiPanels::drawObjectItemInList(Object& object, int object_id)
/** Draws a collapsing header for every object created in the session. Collapsing header includes:
    - color of the object;
    - reset button that resets all changes to objects position/size and returns it to the center of the window;
    - select checkbox. */
{
    std::string object_name = object.ObjectTypeToString() + "_" + object.ObjectIdToString();
    bool show_object = true;

    if (ImGui::CollapsingHeader(object_name.c_str(), &show_object))
    {
        ImGui::Text("Color of the object:");

        auto col = object.getObjectColor();
        std::string edit_col_name = "##fill color_" + object.ObjectIdToString();
        if (ImGui::ColorEdit3(edit_col_name.c_str(), col)){
            object.updateObjectColor();
        }

        std::string button_name = "Reset##"+ object.ObjectIdToString();
        if (ImGui::Button(button_name.c_str())){
            object.resetObjectVertices();
        }
        if (ImGui::IsItemHovered()){
            ImGui::SetTooltip("Reset all changes to object size/position.");
        }

        ImGui::SameLine();
        std::string checkbox_name = "Select##"+ object.ObjectIdToString();
        ImGui::Checkbox(checkbox_name.c_str(), &object.getSelected());
    }
    if (!show_object)
    {
        session_.remove_object(object_id);
    }
}

void GuiPanels::drawSettingsTab()
/** Draws a tab with general settings applicable to all objects:
    - show metadata text under each object;
    - lock position of individual windows to corresponding objects;
    - change rotation sensitivity of objects (objects can be rotated with right mouse button). */
{
    if (ImGui::BeginTabItem("Settings"))
    {
        ImGui::Checkbox("Show metadata", &Config::getParameters().show_metadata);
        ImGui::Spacing();

        if (ImGui::Checkbox("Lock individual panels to objects", &Config::getParameters().lock_gui_to_objects))
        {
            std::string logger_message = "Individual object windows are locked to objects.";
            Logger::addMessage(LogLevel::Debug, logger_message.c_str());
        };
        ImGui::Spacing();

        ImGui::Text("Rotation sensitivity:");
        ImGui::SliderFloat("##rotation_sensitivity", &Config::getParameters().rotation_sensitivity, 0.1f, 2.0f, "ratio = %.1f");

        ImGui::EndTabItem();
    }
}

void GuiPanels::drawHelpTab()
/** Prints README.txt file content with information related to all functionality in this project. */
{
    if (ImGui::BeginTabItem("Help"))
    {
        ImGui::TextWrapped("%s", readme_txt_.c_str());
        ImGui::EndTabItem();
    }
}

void GuiPanels::drawObjectsPanels()
/** Iterates through the vector of objects in the session and if object's boolean object_gui_ is True,
it draws individual panel for this object. */
{
    auto& objects = session_.getObjects();
    for (auto & object : objects)
    {
        drawIndividualPanel(object);
    }
}

void GuiPanels::drawIndividualPanel(Object &object) const
/** Draws individual panel for an object where individual metadata is printed and following settings can be changed:
    - color of the object;
    - polygon mode: line or fill in (with color);
    - zoom factor (for an object);
    - comment (text field). */
{
    auto& gui_parameters = object.getObjectGuiParameters();

    if (gui_parameters.object_gui_)
    {
        ImGui::SetNextWindowSize(ImVec2(object_panel_width_, object_panel_height_));
        if (!object.isPositionInitialized())
        {
            // window position is defined only when window is opened to make it possible to move it.
            // Otherwise, the window will stick to this location.
            ImGui::SetNextWindowPos(ImVec2(gui_parameters.window_x_, gui_parameters.window_y_));
            object.initializePosition();
        }

        std::string gui_window_name = "Panel: Object " + object.ObjectIdToString();
        ImGui::Begin(gui_window_name.c_str(), &gui_parameters.object_gui_, ImGuiWindowFlags_NoResize);

        // Coordinates of an individual window should be saved every frame. The window can be moved freely or it can
        // follow its object. To change window's coordinates along with object's coordinates, its latest position
        // should be available.
        auto current_position = ImGui::GetWindowPos();
        gui_parameters.window_x_ = current_position.x;
        gui_parameters.window_y_ = current_position.y;

        std::string metadata_message = "Object id: " + object.ObjectIdToString() +
                                       "\nObject type: " + object.ObjectTypeToString();
        ImGui::Text("%s", metadata_message.c_str());
        ImGui::Spacing();

        auto col = object.getObjectColor();
        std::string edit_col_name = "##individual fill color_" + object.ObjectIdToString();
        if (ImGui::ColorEdit3(edit_col_name.c_str(), col)){
            object.updateObjectColor();
        }
        ImGui::Spacing();

        auto& polygon_mode = object.getPolygonMode();
        ImGui::RadioButton("fill in", reinterpret_cast<int *>(&polygon_mode), kPolygonModeFill); ImGui::SameLine();
        ImGui::RadioButton("line", reinterpret_cast<int *>(&polygon_mode), kPolygonModeLine);
        ImGui::Spacing();

        std::string zoom_factor_slider = "##zoom_object_"+ object.ObjectIdToString();
        if (ImGui::SliderFloat(zoom_factor_slider.c_str(), &gui_parameters.zoom_factor_, 0.1f, 2.5f, "zoom = %.1f")){
            object.calculateBoundingBox();
        }
        ImGui::Spacing();

        ImGui::Text("Comment:");
        std::string comment_input = "##comment_input"+ object.ObjectIdToString();
        ImGui::InputText(comment_input.c_str(), gui_parameters.comment_, sizeof(gui_parameters.comment_[0]) * 128);

        ImGui::End();
    }
}

void GuiPanels::drawLoggerTab()
/** Prints all logger messages in the Logger tab. */
{
    if (ImGui::BeginTabItem("Logger"))
    {
        Logger::drawLogger();
        ImGui::EndTabItem();
    }
}

std::string GuiPanels::readTextFile(const std::string& filePath)
/** Reads the contents of a text file into a string. */
{
    std::ifstream file(filePath);
    std::stringstream buffer;

    if (file.is_open())
    {
        buffer << file.rdbuf();
        file.close();
    } else {
        return "Failed to open the file.";
    }

    return buffer.str();
}

