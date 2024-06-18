#include <algorithm>
#include "logger.h"

#include "../include/session.h"


void Session::loadAllObjectsBuffers()
/** Iterates through the vector of objects and applies member function to load buffers for every object. */
{
    for (auto& object: objects_){
        object.loadObjectBuffers();
    }
}

void Session::add_object(ObjectType object_type)
/** Creates an instance of an Object class with a specified type (cube, pyramid etc), generates pick color to enable
manipulations with drawn object. Then adds it to the objects_ vector.
Also, adds an info message to logger with id and type of created object. */
{
    current_object_id_ = current_object_id_ + 1;
    generateNewPickColor_();

    auto pick_color_id = generatePickColorID_();
    auto new_object = Object(current_object_id_, pick_color_id, object_type, 1, 0,0, current_pick_color_[0], current_pick_color_[1], current_pick_color_[2]);

    new_object.loadObjectBuffers();
    objects_.push_back(std::move(new_object));

    std::string logger_message = "An object type " + new_object.ObjectTypeToString() + " is created.";
    Logger::addMessage(LogLevel::Info, logger_message.c_str());
}

void Session::drawAllObjects(bool get_pick_color)
/** Iterates through the vector of objects and applies member function to draw every object. */
{
    for (auto& object: objects_)
    {
        object.draw(get_pick_color);
    }
}

void Session::drawAllObjectsMetadata()
/** Iterates through the vector of objects and applies member function to draw (print) metadata of every object. */
{
    for (auto& object: objects_)
    {
        object.drawMetadataText();
    }
}

void Session::generateNewPickColor_()
/** Generates a new pick color by incrementing the RGB values by 10.
If a component reaches 255, it resets to 0 and increments the previous component. */
{
    for (int i = 2; i >= 0; --i)
    {
        if (current_pick_color_[i] < 255)
        {
            current_pick_color_[i] += 10;
            return;
        }
        if (current_pick_color_[i] == 255)
        {
            current_pick_color_[i] = 0;
            current_pick_color_[i - 1]++;
            return;
        }
    }
}

int Session::generatePickColorID_()
/** Calculates pick color id using with the formula: R*256^2 + G*256^1 + B*256^0. Afterwards, pick_color_id is assigned
to an Object instance. */
{
    int pick_color_id = current_pick_color_[0] * (256 * 256) + current_pick_color_[1] * 256 +
                        current_pick_color_[2];
    return pick_color_id;
}

int Session::getObjectIdByPickColor(const unsigned char* pick_color)
/** Calculates pick_color_id for a colour values (see formula above), iterates through the vector of Objects to get an id
of an Object with specified pick colour. */
{
    int pick_color_id = pick_color[0] * (256 * 256) + pick_color[1] * 256 + pick_color[2];
    for (std::vector<Object>::size_type i = 0; i < objects_.size(); i++)
    {
        if (objects_[i].checkPickColor(pick_color_id))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void Session::reset()
/** Iterates through the vector of objects and applies member function to reset every object. */
{
    for (auto& object: objects_)
    {
        object.reset();
    }
}

void Session::updateObjectsCoordinates(const std::vector<int>& object_ids, double delta_x, double delta_y)
/** Iterates through the vector of object_ids and applies Object member function to update vertices coordinates
of the corresponding Objects. It moves x- and y- coordinates by x_delta and y_delta of mouse cursor position. */
{
    for (auto object_id : object_ids){
        objects_[object_id].updateObjectCoordinates(delta_x, delta_y);
    }
}

void Session::remove_object(int object_id)
/** If a vector of Objects is not longer than provided object id, then object with specifies id is erased from vector.
Technically every Object in the vector objects_ has 2 ids:
    - index in the vector objects_;
    - id assigned to Object when the instance is created.
They are not always the same. Index in the vector objects_ is used to define which Object to remove. */
{
    std::string logger_message = "An object #" + objects_[object_id].ObjectIdToString() +
                                " type " + objects_[object_id].ObjectTypeToString() +
                                " is removed.";
    if (objects_.size() > object_id)
    {
        objects_.erase(objects_.begin() + object_id);
    }

    Logger::addMessage(LogLevel::Info, logger_message.c_str());
}

void Session::updateObjectsRotation(const std::vector<int>& object_ids, double delta_x, double delta_y)
/** Iterates through the vector of object_ids and applies an Object member function
to update the vertices coordinates of the corresponding Objects.
It calculates the center of the Object, the angle of rotation based on the x_delta and y_delta
of the mouse cursor position, and rotates the coordinates of the vertices by the calculated angle. */
{
    for (auto object_id : object_ids){
        objects_[object_id].updateObjectRotation(delta_x, delta_y);
    }
}

std::vector<int> Session::getSelectedObjects()
/** Iterates through the vector of Objects, if Object's variable selected_ is true, adds its id to the vector
selected_objects_id and returns this vector. */
{
    std::vector<int> selected_objects_id{};

    for (int i=0; i < objects_.size(); i += 1)
    {
        if (objects_[i].getSelected())
        {
            selected_objects_id.push_back(i);
        }
    }

    return selected_objects_id;
}

void Session::deSelectAllObjects()
/** Iterates through the vector of Objects and if Object's variable selected_ is true, switch it to false. */
{
    for (auto &object: objects_)
    {
        if (object.getSelected()){
            object.switchSelected();
        }
    }
}

void Session::selectAllObjects()
/** Iterates through the vector of Objects and if Object's variable selected_ is false, switch it to true. */
{
    for (auto &object: objects_)
    {
        if (!object.getSelected()){
            object.switchSelected();
        }
    }
}

void Session::selectObjectsInFrame(const std::vector<int> &object_ids)
/** Iterates through the vector of object_ids and if Object's variable selected_ is false, switch it to true. */
{
    for (auto object_id : object_ids){
        if (!objects_[object_id].getSelected()) {
            objects_[object_id].switchSelected();
        }

    }
}

void Session::updateObjectsGuiCoordinates(const std::vector<int> &object_ids, float window_width, float window_height,
                                         double delta_x, double delta_y)
/** Iterates through the vector of object_ids and applies Object member function to update coordinates of individual
ImGui panel of an Object. Main window height and width are taken into consideration to keep it in window limits. */
{
    for (auto object_id : object_ids){
        objects_[object_id].updateGuiWindowDeltaCoordinates(window_width, window_height, delta_x, delta_y);
    }
}


