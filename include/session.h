#ifndef PROJECT_1_SESSION_H
#define PROJECT_1_SESSION_H

#include "../include/object.h"

class Session
/* Class Session contains all Object instances created in a session of application. Application manipulates objects
through Session by object id.*/
{
public:
    void add_object(ObjectType object_type);
    void remove_object(int object_id);

    void loadAllObjectsBuffers();
    void drawAllObjects(bool get_pick_color);
    void drawAllObjectsMetadata();
    int getObjectIdByPickColor(const unsigned char* pick_color);

    void reset();

    void updateObjectsCoordinates(const std::vector<int>& object_ids, double delta_x, double delta_y);
    void updateObjectsRotation(const std::vector<int>& object_ids, double delta_x, double delta_y);
    void updateObjectsGuiCoordinates(const std::vector<int>& object_ids, float window_width, float window_height, double delta_x, double delta_y);

    std::vector<Object>& getObjects(){return objects_;};
    std::vector<int> getSelectedObjects();

    void selectAllObjects();
    void deSelectAllObjects();

    void selectObjectsInFrame(const std::vector<int> &object_ids);


private:
    std::vector<Object> objects_;
    int current_object_id_{0};
    int current_pick_color_[3]{0, 0, 0};

    int generatePickColorID_();
    void generateNewPickColor_();
};


#endif //PROJECT_1_SESSION_H
