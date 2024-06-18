#ifndef PROJECT_1_OBJECT_H
#define PROJECT_1_OBJECT_H

#include <iostream>
#include "vector"
#include "array"
#include "tuple"
#include <GLFW/glfw3.h>


enum ObjectType
{
    kCube = 0,
    kPyramid = 1,
    kSphere = 2,
    kIcosahedron = 3
};

enum PolygonMode
{
    kPolygonModeLine  = GL_LINE,
    kPolygonModeFill  = GL_FILL,
};

struct BoundingBox
{
    float minX;
    float maxX;
    float minY;
    float maxY;
};

struct GuiParameters
{
    double window_x_{};
    double window_y_{};
    bool object_gui_{false};
    char comment_[128];
    float zoom_factor_{1};
};

class Object
{
public:
    explicit Object(int id, int pick_id, ObjectType object_type, GLfloat r, GLfloat g, GLfloat b, GLubyte pick_r, GLubyte pick_g, GLubyte pick_b);
    void draw(bool get_pick_color = false);
    void drawMetadataText() const;

    void reset();
    void loadObjectBuffers();
    bool checkPickColor(int pick_color_id) const;

    std::string ObjectTypeToString() const;
    std::string ObjectIdToString() const;

    void updateObjectCoordinates(double delta_x, double delta_y);
    void updateObjectRotation(double delta_x, double delta_y);
    void updateObjectColor();
    void resetObjectVertices();

    float* getObjectColor(){return rgb_;}
    bool& getSelected(){return selected_;}
    PolygonMode& getPolygonMode(){return polygon_mode_;}

    void switchSelected(){selected_ = !selected_;}
    void switchGuiEnabled(){gui_parameters_.object_gui_ = !gui_parameters_.object_gui_; is_position_initialized_ = false;}

    void calculateBoundingBox();

    void setGuiWindowCoordinates(float window_width, float window_height, double x, double y);
    void updateGuiWindowDeltaCoordinates(float window_width, float window_height, double delta_x, double delta_y);

    GuiParameters& getObjectGuiParameters(){return gui_parameters_;}
    void initializePosition(){is_position_initialized_ = true;}
    bool isPositionInitialized() const{return is_position_initialized_;}

private:
    float pi_ = 3.14159265359f;

    int id_;
    int pick_id_;
    float rgb_[3];
    bool selected_{false};

    std::vector<GLfloat> vertices_{};
    std::vector<GLuint> indices_{};
    std::vector<GLfloat> colours_{};
    std::vector<GLubyte> pick_colours_{};

    ObjectType object_type_;
    BoundingBox bounding_box_;

    GLuint vertex_buffer_object_{};
    GLuint index_buffer_object_{};
    GLuint color_buffer_object_{};
    GLuint pick_color_buffer_object_{};

    PolygonMode polygon_mode_;
    GuiParameters gui_parameters_;

    bool is_position_initialized_ = false;

    void drawDefault_();
    void drawWithPick_();
    std::array<float, 3>  calculateCenter();
};

#endif //PROJECT_1_OBJECT_H
