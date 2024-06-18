#ifndef PROJECT_1_GUI_PANELS_H
#define PROJECT_1_GUI_PANELS_H

#include "map"
#include "imgui.h"
#include "../include/session.h"
#include "../include/object.h"


class GuiPanels
{
public:
    explicit GuiPanels(Session& session) : session_(session){ readme_txt_ = readTextFile("../ReadMe.txt");};
    void drawMainPanel();
    void drawObjectsPanels();

private:
    Session& session_;

    void drawObjectsTab();
    void drawCreateObjects();
    void drawObjectsList();
    void drawObjectItemInList(Object& object, int object_id);
    static void drawSettingsTab();
    void drawHelpTab();
    void drawIndividualPanel(Object& object) const;
    static void drawLoggerTab();
    std::string readTextFile(const std::string& filePath);

    std::string readme_txt_;
    std::string current_obj_type_{"Cube"};
    const std::map<std::string, ObjectType> object_types_ = {
            { "Cube", kCube },
            { "Pyramid", kPyramid },
            { "Sphere", kSphere },
            { "Icosahedron", kIcosahedron }
    };

    float object_panel_height_ = 210.0f;
    float object_panel_width_ = 230.0f;
};

#endif //PROJECT_1_GUI_PANELS_H
