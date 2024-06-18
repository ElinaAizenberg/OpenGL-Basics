
#ifndef PROJECT_1_LIBRARY_H
#define PROJECT_1_LIBRARY_H

#include <iostream>
#include <cmath>
#include "vector"
#include <GLFW/glfw3.h>


const float pi = 3.14159265359f;

// Polyhedron struct contains vertices and indices to create Object class instances and draw with OpenGL functions.
struct Polyhedron{
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};


Polyhedron cube = {
        {
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f
        },
        {
                // Back face
                0, 1, 2,
                2, 3, 0,
                4, 5, 6,
                6, 7, 4,
                0, 3, 7,
                7, 4, 0,
                1, 5, 6,
                6, 2, 1,
                3, 2, 6,
                6, 7, 3,
                0, 1, 5,
                5, 4, 0
        }
};

Polyhedron pyramid = {
        {
                -0.5f, 0.0f,-0.5f,
                0.5f, 0.0f, -0.5f,
                0.5f, 0.0f,0.5f,
                -0.5f, 0.0f, 0.5f,
                0.0f, 1.0f ,0.0f,
        },
        {
                0, 1, 4,  // Side Face 0
                1, 2, 4,  // Side Face 1
                2, 3, 4,  // Side Face 2
                3, 0, 4,  // Side Face 3
                0, 1, 2,  // Base Face (Triangle 1)
                2, 3, 0   // Base Face (Triangle 2)
        }
};

Polyhedron generateSphere(float precision)
/* Generate Polyhedron struct with vertices and indices to draw a sphere.*/
{
    Polyhedron sphere;

    for (int i = 0; i <= precision; ++i) {
        float theta1 = i * pi / precision;
        float theta2 = (i + 1) * pi / precision;

        for (int j = 0; j <= precision * 2; ++j) {
            float phi = j * 2 * pi / (precision * 2);

            float x1 = sin(theta1) * cos(phi);
            float y1 = cos(theta1);
            float z1 = sin(theta1) * sin(phi);

            float x2 = sin(theta2) * cos(phi);
            float y2 = cos(theta2);
            float z2 = sin(theta2) * sin(phi);

            sphere.vertices.push_back(x1);
            sphere.vertices.push_back(y1);
            sphere.vertices.push_back(z1);

            sphere.vertices.push_back(x2);
            sphere.vertices.push_back(y2);
            sphere.vertices.push_back(z2);
        }
    }


    for (unsigned int i = 0; i < sphere.vertices.size() / 3 - precision - 1; i += (precision + 1)) {
        for (int j = 0; j < precision; ++j) {
            sphere.indices.push_back(i + j);
            sphere.indices.push_back(i + j + 1);
            sphere.indices.push_back(i + j + 2);

            sphere.indices.push_back(i + j + 2);
            sphere.indices.push_back(i + j + 1);
            sphere.indices.push_back(i + j + 3);
        }
    }

    return sphere;
}

Polyhedron icosahedron = {
        {-0.5257311, 0.0, 0.8506508,
         0.5257311, 0.0, 0.8506508,
         -0.5257311, 0.0, -0.8506508,
         0.5257311, 0.0, -0.8506508,
         0.0, 0.8506508, 0.5257311,
         0.0, 0.8506508, -0.5257311,
         0.0, -0.8506508, 0.5257311,
         0.0, -0.8506508, -0.5257311,
         0.8506508, 0.5257311, 0.0,
         -0.8506508, 0.5257311, 0.0,
         0.8506508, -0.5257311, 0.0,
         -0.8506508, -0.5257311, 0.0
        },
        {
                0,4,1,
                0,9,4,
                9,5,4,
                4,5,8,
                4,8,1,
                8,10,1,
                8,3,10,
                5,3,8,
                5,2,3,
                2,7,3,
                7,10,3,
                7,6,10,
                7,11,6,
                11,0,6,
                0,1,6,
                6,1,10,
                9,0,11,
                9,11,2,
                9,2,5,
                7,2,11
        }
};


Polyhedron sphere = generateSphere(40);
std::vector<Polyhedron> allPolyhedronTypes = {cube, pyramid, sphere, icosahedron};

Polyhedron getPolyhedronByType(int type_id)
/** Returns Polyhedron struct by object type_id. If id exceeds size of the objects' library, it returns Cube. */
{
    if (type_id <= allPolyhedronTypes.size())
    {
        return allPolyhedronTypes[type_id];
    }
    return allPolyhedronTypes[0];
}

#endif //PROJECT_1_LIBRARY_H
