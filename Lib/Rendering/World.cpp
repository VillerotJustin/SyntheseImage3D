//
// Created by villerot on 03/10/2025.
//

#include "World.h"

namespace rendering {

    World::World()
        : objects(math::Vector<ShapeVariant>(0)),
          camera(Rectangle(Vector3D(0, 0, 0), 100.0, 100.0, Vector3D(0, 0, -1)))
    {
        // Default constructor - initialize empty world
        // Camera rectangle initialized in initializer list
    }

    int World::getObjectCount() const {
        return objects.size();
    }

    Camera& World::getCamera() {
        return camera;
    }

    const Camera& World::getCamera() const {
        return camera;
    }

    void World::clearObjects() {
        // Delete all objects and clear the vector
        for (int i = 0; i < objects.size(); ++i) {
            if (objects[i] != nullptr) {
                delete objects[i];
            }
        }
        objects.clear();
    }

    Image World::renderScene2DColor(int imageWidth, int imageHeight) const {
        // Dispatch rendering based on the type of shapes in the world
        // For simplicity, we assume all shapes are of the same type here
        if (objects.size() == 0) {
            return Image(imageWidth, imageHeight); // Return empty image if no objects
        }

        return camera.renderScene2DColor(imageWidth, imageHeight, objects);
    }

} // namespace rendering
