#ifndef PROJECT_1_CONFIG_H
#define PROJECT_1_CONFIG_H

// Parameters struct contain variables that are used by both ImGui windows (Main panel and individual objects' panel)
// and by Drawing library to draw OpenGL objects.
struct Parameters
{
    bool show_metadata{false};
    bool lock_gui_to_objects{false};
    float rotation_sensitivity{0.5};
};

class Config
/** Config class is used across the whole application to get access to Parameters. */
{
public:
    static Parameters& getParameters(){return parameters_;}

private:
    static Parameters parameters_;
};


#endif //PROJECT_1_CONFIG_H
