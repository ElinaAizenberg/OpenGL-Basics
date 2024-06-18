#include <GL/glew.h>
#include <tuple>
#include <cstring>

#include "../include/object.h"
#include "../include/config.h"
#include "../include/library.h"
#include "../include/font.h"

Object::Object(int id, int pick_id, ObjectType object_type, GLfloat r, GLfloat g, GLfloat b, GLubyte pick_r, GLubyte pick_g, GLubyte pick_b): id_(id), pick_id_(pick_id), object_type_(object_type) {
    // By default, an Object is drawn filled with color, therefore uses parameter GL_FILL.
    polygon_mode_ = kPolygonModeFill;

    // Initialize the comment buffer to an empty string.
    std::memset(gui_parameters_.comment_, 0, sizeof(gui_parameters_.comment_));

    rgb_[0] = r;
    rgb_[1] = g;
    rgb_[2] = b;

    // glGenBuffers generates buffers for later rendering.
    // A buffer in OpenGL is, at its core, an object that manages a certain piece of GPU memory.
    // In this project an Object is drawn without lighting, otherwise an additional buffer for normals is needed.
    // This data can be vertex coordinates, indices, texture coordinates, normals, colors, etc.
    glGenBuffers(1, &vertex_buffer_object_);
    glGenBuffers(1, &index_buffer_object_);
    glGenBuffers(1, &color_buffer_object_);
    glGenBuffers(1, &pick_color_buffer_object_);

    // Get a struct with vertices and indices from library.h.
    Polyhedron object_sample = getPolyhedronByType(object_type);

    // Every coordinate (x, y, z) of a vertex needs to have corresponding rgb values.
    // A vector and buffer for pick colours are used to manipulate (move, rotate, select) with Objects.
    for (size_t i = 0; i < object_sample.vertices.size(); i += 1)
    {
        colours_.push_back(r);
        colours_.push_back(g);
        colours_.push_back(b);

        pick_colours_.push_back(pick_r);
        pick_colours_.push_back(pick_g);
        pick_colours_.push_back(pick_b);
    }

    // copy vertices and indices from object_sample into Object's variables.
    vertices_.insert(vertices_.end(), object_sample.vertices.begin(), object_sample.vertices.end());
    indices_.insert(indices_.end(), object_sample.indices.begin(), object_sample.indices.end());

    // Calculate bounding box to draw metadata text below the object.
    // Bounding box is re-calculated every time vertices change.
    calculateBoundingBox();

}

void Object::reset()
/** Resets all buffers (vertices, indices, colours and pick_colours) of an Object. */
{
    // glDeleteBuffers deletes buffer objects.
    // After a buffer object is deleted, it has no contents, and its name is free for reuse
    glDeleteBuffers(1, &vertex_buffer_object_);
    glDeleteBuffers(1, &index_buffer_object_);
    glDeleteBuffers(1, &color_buffer_object_);
    glDeleteBuffers(1, &pick_color_buffer_object_);
}

void Object::loadObjectBuffers()
/** Loads data into all Object's buffers: (vertices, indices, colours and pick_colours. */
{
    // glBindBuffer binds a buffer object to the target GL_ARRAY_BUFFER. It means that this buffer  will be used
    // for subsequent operations. This binding remains active until another buffer is bound to the same target, or the buffer is unbound.
    // Target is a symbolic constant used to specify the type of buffer object that will store attribute data.
    // GL_ARRAY_BUFFER is a target to store vertex attribute data (coordinates, texture coord., normals, colours, etc).
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

    // glBufferData creates and initializes the buffer object's data store.
    // Size - the total size needed for the vertex data calculated by multiplying the size of a single GLfloat
    // by the number of vertices.
    // GL_STATIC_DRAW - usage pattern of the data store, means the data will be set once and used many times for drawing operations.
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);

    // GL_ELEMENT_ARRAY_BUFFER is a target to store indices of each element in the "other" (GL_ARRAY_BUFFER) buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint) * indices_.size(),
                 indices_.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * colours_.size(),
                 colours_.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, pick_color_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLubyte) * pick_colours_.size(),
                 pick_colours_.data(),
                 GL_STATIC_DRAW);
}


void Object::draw(bool get_pick_color)
/** High-level drawing function that selects whether to draw using general colours buffer or pick_colors buffer. */
{
    if (get_pick_color)
    {
        drawWithPick_();
    } else {
        drawDefault_();
    }
}

void Object::drawDefault_()
/** Renders an Object using OpenGL. It sets up the rendering mode to draw the Object's polygons and colors.
If the object is selected, it modifies the line color to green and applies a stipple pattern. */
{
    // Save the current transformation matrix to ensure that subsequent glScalef doesn't affect other Objects.
    glPushMatrix();

    // Apply scaling with zooming factor.
    glScalef(gui_parameters_.zoom_factor_, gui_parameters_.zoom_factor_, gui_parameters_.zoom_factor_);

    // glPolygonMode sets the polygon drawing mode, determining how polygons will be rasterized.
    // GL_FRONT_AND_BACK applies the mode to both front and back faces of polygons.
    // Mode can be set as GL_FILL, GL_LINE, or GL_POINT.
    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode_);

    // Enables OpenGL to use the array of vertices specified later.
    glEnableClientState(GL_VERTEX_ARRAY);
    // Binds the vertex buffer object (VBO) to the GL_ARRAY_BUFFER target.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    // Specifies the location and data format of the array.
    // It tells OpenGL that the vertex array data consists of 3-component (x, y, z) vertices of type GL_FLOAT.
    // The stride and offset are both 0, meaning the data is tightly packed without gaps.
    glVertexPointer(3, GL_FLOAT, 0, 0);

    // Enables the client-side capability to use color arrays.
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_object_);
    // Specifies the location and data format of the array of colors.
    // It tells OpenGL that the color array data consists of 3-component (R, G, B) colors of type GL_FLOAT.
    glColorPointer(3, GL_FLOAT, 0, 0);

    // Bind the index buffer object (IBO) to the GL_ELEMENT_ARRAY_BUFFER target.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);

    // Renders primitives from array data.
    // Due to mode GL_TRIANGLES it draws triangles using the indices stored in the index buffer object.
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);

    // Disables the client-side capability to use color arrays.
    // This is a cleanup step to ensure that color arrays are not used unintentionally in subsequent rendering operations.
    glDisableClientState(GL_COLOR_ARRAY);

    // Set polygon mode to draw lines.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1, 1, 1);  // Set color to white

    if (selected_) // If the Object is selected, set color to green, increase line width to 2.0 and enable line
        // stipple to create a dashed line effect.
    {
        glColor3f(0, 1, 0);
        glLineWidth(2.0f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF); // 0x00FF is the pattern, 1 is the repeat factor
    }
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);
    glDisable(GL_LINE_STIPPLE); // Disable the line stipple effect
    glLineWidth(1.0f); // Reset line width to default

    glDisableClientState(GL_VERTEX_ARRAY);  // Disable the client-side capability to use vertex arrays

    // Restore the previous transformation matrix saved with glPushMatrix.
    glPopMatrix();


}

void Object::drawWithPick_()
/** Repeats drawDefault_ method above, but applies pick_color_buffer_object_ buffer instead of
general color_buffer_object_ to color Object's polygons, and applies only GL_FILL polygon mode.
Every Object despite general color RGB values (that can be the same for multiple Objects) has unique pick colour values.
Rendering Objects with pick colours is used to manipulate with Objects and detect which Object
is selected in the window.*/
{
    glPushMatrix();
    glScalef(gui_parameters_.zoom_factor_, gui_parameters_.zoom_factor_, gui_parameters_.zoom_factor_);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, pick_color_buffer_object_);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices_.size()), GL_UNSIGNED_INT, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glPopMatrix();

}

std::string Object::ObjectTypeToString() const
/** Returns regular name of the Object based on its enum ObjectType value.*/
{
    switch (object_type_)
    {
        case kCube: return "Cube";
        case kPyramid: return "Pyramid";
        case kSphere: return "Sphere";
        case kIcosahedron: return "Icosahedron";
        default: return "Unknown";
    }
}

bool Object::checkPickColor(int pick_color_id) const
/** Checks if Object's pick_color_id is equal to the provided id. */
{
    if (pick_color_id == pick_id_)
    {
        return true;
    }
    return false;
}

void Object::updateObjectCoordinates(double delta_x, double delta_y)
/** Iterates through vertices and change x- and y- coordinates by delta_x and delta_y. Re-load vertices buffer.
Delta-x and delta-y are calculated based on changes of mouse cursor position and OpenGL viewport parameters.*/
{
    for (size_t i = 0; i < vertices_.size(); i += 3)
    {
        vertices_[i] += static_cast<float>(delta_x);
        // Subtract delta_y because screen y-coordinates go top-to-bottom,
        // and OpenGL viewport y-coordinates go bottom-to-top.
        vertices_[i + 1] -= static_cast<float>(delta_y);
        vertices_[i + 2] += 0.0f;  // Ensure z-coordinate remains unchanged
    }

    // When vertices change, bounding box needs to be re-calculated.
    calculateBoundingBox();

    // When vertices change, buffer with vertices data has to be reset and new data stored again.
    // Changes in coordinates of vertices do not affect other buffers: indices, colours and pick_colours.
    glDeleteBuffers(1, &vertex_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
}

std::string Object::ObjectIdToString() const
/** Returns Object's id value in string format.*/
{
    return std::to_string(id_);
}

void Object::updateObjectColor()
/** Iterates through vertices and assign new rgb values foe every vertex. Re-load colours buffer. */
{
    colours_.clear();

    for (size_t i = 0; i < vertices_.size(); i += 1)
    {
        colours_.push_back(rgb_[0]);
        colours_.push_back(rgb_[1]);
        colours_.push_back(rgb_[2]);
    }
    // When rgb values for vertices change, buffer with colours data has to be reset and new data stored again.
    glDeleteBuffers(1, &color_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * colours_.size(),
                 colours_.data(),
                 GL_STATIC_DRAW);
}


std::array<float, 3> Object::calculateCenter()
/** Calculates the average coordinates of the Object's vertices to approximate the center.
This method suits to find the center of an object when the vertices are uniformly distributed around the center.
For irregular shapes the centroid calculation method is preferred (calculate centroid of every triangle and calculate
weighted average of all centroids. */
{
    std::array<float, 3> center = {0.0f, 0.0f, 0.0f};

    for (size_t i = 0; i < vertices_.size(); i += 3) {
        center[0] += vertices_[i];
        center[1] += vertices_[i + 1];
        center[2] += vertices_[i + 2];
    }
    float vertex_count = static_cast<float>(vertices_.size()) / 3;

    center[0] /= vertex_count;
    center[1] /= vertex_count;
    center[2] /= vertex_count;

    return center;
}

void Object::updateObjectRotation(double delta_x, double delta_y)
/** Updates the rotation of the object based on mouse movement.
Delta-x and delta-y are calculated based on changes of mouse cursor position
It computes the new rotation angles from the deltas, converts them to radians, and applies the rotations
to the object's vertices around its center. */
{
    // Delta_x and delta_y are scaled by a rotation sensitivity parameter and converted from degrees to radians.
    float radians_x = delta_y * Config::getParameters().rotation_sensitivity * pi_ / 180.0f;
    float radians_y = -delta_x * Config::getParameters().rotation_sensitivity * pi_ / 180.0f;

    // Compute the sin and cos of the rotation angles
    float cos_x = cos(radians_x);
    float sin_x = sin(radians_x);
    float cos_y = cos(radians_y);
    float sin_y = sin(radians_y);

    // Calculate approximate center.
    auto center = calculateCenter();

    for (size_t i = 0; i < vertices_.size(); i += 3)
    {
        // Each vertex is translated such that the center of the object becomes the origin.
        float x = vertices_[i] - center[0];
        float y = vertices_[i + 1] - center[1];
        float z = vertices_[i + 2] - center[2];

        // Rotate around the y-axis (horizontal mouse movement)
        float temp_x = x * cos_y - z * sin_y;
        float temp_z = x * sin_y + z * cos_y;

        // Update the vertex coordinates after y rotation
        x = temp_x;
        z = temp_z;

        // Rotate around the x-axis (vertical mouse movement)
        float temp_y = y * cos_x - z * sin_x;
        z = y * sin_x + z * cos_x;

        // The rotated vertex is translated back to its original position relative to the center.
        vertices_[i] = x + center[0];
        vertices_[i+1] = temp_y + center[1];
        vertices_[i+2] = z + center[2];
    }
    // When vertices change, bounding box needs to be re-calculated.
    calculateBoundingBox();

    // When vertices change, buffer with vertices data has to be reset and new data stored again.
    // Changes in coordinates of vertices do not affect other buffers: indices, colours and pick_colours.
    glDeleteBuffers(1, &vertex_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
}

void Object::resetObjectVertices()
/** Re-load all vertices from a sample object from library.h and reset vertices buffer
 to return Object to the default position and size. Re-calculates bounding box around the Object.*/
{
    gui_parameters_.zoom_factor_ = 1;
    vertices_.clear();

    Polyhedron object_sample = getPolyhedronByType(object_type_);
    vertices_.insert(vertices_.end(), object_sample.vertices.begin(), object_sample.vertices.end());

    glDeleteBuffers(1, &vertex_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);

    calculateBoundingBox();
}

void Object::calculateBoundingBox()
/** Calculates the bounding box of the Object based on its vertices.
Iterates through all the vertices of the object to determine the minimum and maximum x and y coordinates,
then adjusts them according to the current zoom factor from the individual ImGui window parameters.*/
{
    bounding_box_.minX = bounding_box_.maxX = vertices_[0];
    bounding_box_.minY = bounding_box_.maxY = vertices_[1];

    for (size_t i = 0; i < vertices_.size(); i += 3)
    {
        float x = vertices_[i];
        float y = vertices_[i + 1];
        if (x < bounding_box_.minX) {bounding_box_.minX = x;}
        if (x > bounding_box_.maxX) {bounding_box_.maxX = x;}
        if (y < bounding_box_.minY) {bounding_box_.minY = y;}
        if (y > bounding_box_.maxY) {bounding_box_.maxY = y;}
    }
    bounding_box_.minY *= gui_parameters_.zoom_factor_;
    bounding_box_.minX *= gui_parameters_.zoom_factor_;
    bounding_box_.maxX *= gui_parameters_.zoom_factor_;
    bounding_box_.maxY *= gui_parameters_.zoom_factor_;
}

void Object::setGuiWindowCoordinates(float window_width, float window_height, double x, double y)
/** Defines ImGui window position based on cursor coordinates. Adjust coordinates according to main window parameters
to ensure that window doesn't go beyond window edges.*/
{
    // Calculate desired position (slightly above the cursor)
    gui_parameters_.window_x_ = x - (230 / 2.0f); // Center horizontally over the cursor, 230 - ImGui window width
    gui_parameters_.window_y_ = y - 200; // 200 pixels above the cursor

    // Adjust position to fit within screen bounds.
    // Horizontal adjustment
    if (gui_parameters_.window_x_ < 0)
    {
        gui_parameters_.window_x_ = 0; // Ensure window doesn't go beyond the left edge
    }
    else if (gui_parameters_.window_x_ + 250 > window_width)
    {
        gui_parameters_.window_x_ = window_width - 250; // Ensure window doesn't go beyond the right edge
    }

    // Vertical adjustment
    if (gui_parameters_.window_y_ < 0)
    {
        gui_parameters_.window_y_ = y + 10; // Position below the cursor if it doesn't fit above
        if (gui_parameters_.window_y_ + 250 > window_height)
        {
            gui_parameters_.window_y_ = window_height - 250; // Ensure window doesn't go beyond the bottom edge
        }
    }
}

void Object::updateGuiWindowDeltaCoordinates(float window_width, float window_height, double delta_x, double delta_y)
/** Update ImGui window position by delta_x and delta_y of mouse cursor position.
Adjust coordinates according to main window parameters to ensure that window doesn't go beyond window edges. */
{
    gui_parameters_.window_x_ += delta_x;
    gui_parameters_.window_y_ += delta_y;

    if (gui_parameters_.window_x_ < 0)
    {
        gui_parameters_.window_x_ = 0; // Ensure window doesn't go beyond the left edge
    }
    else if (gui_parameters_.window_x_ + 250 > window_width)
    {
        gui_parameters_.window_x_ = window_width - 250; // Ensure window doesn't go beyond the right edge
    }

    // Vertical adjustment
    if (gui_parameters_.window_y_ < 0)
    {
        gui_parameters_.window_y_ += 10; // Position below the cursor if it doesn't fit above
        if (gui_parameters_.window_y_ + 250 > window_height)
        {
            gui_parameters_.window_y_ = window_height - 250; // Ensure window doesn't go beyond the bottom edge
        }
    }
    is_position_initialized_ = false;
}

void Object::drawMetadataText() const
/** Draws Object's metadata text. Defines initial raster position based on bounding box parameters and
 uses print_string function from font.h that draws text with glBitmap. */
{
    float raster_pos_x = bounding_box_.minX;
    float raster_pos_y = bounding_box_.minY - 0.5;

    std::string metadata_text = "Object id: " + ObjectIdToString() +
                                "\nObject type: " + ObjectTypeToString() +
                                "\nComment: " + gui_parameters_.comment_;
    glRasterPos2f(raster_pos_x , raster_pos_y);
    print_string(metadata_text.c_str());
}