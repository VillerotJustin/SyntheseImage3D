//
// Created by villerot on 03/10/2025.
//

#include "World.h"

namespace rendering {

    World::World()
        : objects(math::Vector<ShapeVariant>(0)),
          camera(Rectangle(Vector3D(0, 0, 0), Vector3D(0, 100, 0), Vector3D(0, 0, 100)))
    {
        // Default constructor - initialize empty world with default camera
        // Camera positioned at (0,0,5) with 10x10 viewport looking toward origin
    }

    void World::addLight(const Light& light) {
        Light* newLight = new Light(light);
        lights.append(newLight);
    }

    void World::removeLight(const Light& light) {
        for (size_t i = 0; i < lights.size(); ++i) {
            if (*lights[i] == light) {
                delete lights[i];
                lights.erase(i);
                return;
            }
        }
    }

    void World::removeLightAt(const size_t& index) {
        if (index >= lights.size()) {
            throw std::out_of_range("Light index out of bounds");
        }
        delete lights[index];
        lights.erase(index);
    }

    size_t World::getObjectCount() const {
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
        for (size_t i = 0; i < objects.size(); ++i) {
            if (objects[i] != nullptr) {
                delete objects[i];
            }
        }
        objects.clear();
    }

    Image World::renderScene2DColor(size_t imageWidth, size_t imageHeight) const {
        // Dispatch rendering based on the type of shapes in the world
        // For simplicity, we assume all shapes are of the same type here
        if (objects.size() == 0) {
            return Image(imageWidth, imageHeight); // Return empty image if no objects
        }

        return camera.renderScene2DColor(imageWidth, imageHeight, objects);
    }

    Image World::renderScene2DDepth(size_t imageWidth, size_t imageHeight) const {
        // Dispatch rendering based on the type of shapes in the world
        // For simplicity, we assume all shapes are of the same type here
        if (objects.size() == 0) {
            return Image(imageWidth, imageHeight); // Return empty image if no objects
        }

        return camera.renderScene2DDepth(imageWidth, imageHeight, objects);
    }

    Image World::renderScene3DColor(size_t imageWidth, size_t imageHeight) const {
        // Dispatch rendering based on the type of shapes in the world
        // For simplicity, we assume all shapes are of the same type here
        if (objects.size() == 0) {
            return Image(imageWidth, imageHeight); // Return empty image if no objects
        }

        return camera.renderScene3DColor(imageWidth, imageHeight, objects);
    }

    Image World::renderScene3DDepth(size_t imageWidth, size_t imageHeight) const {
        // Dispatch rendering based on the type of shapes in the world
        // For simplicity, we assume all shapes are of the same type here
        if (objects.size() == 0) {
            return Image(imageWidth, imageHeight); // Return empty image if no objects
        }

        return camera.renderScene3DDepth(imageWidth, imageHeight, objects);
    }

    Image World::renderScene3DLight(size_t imageWidth, size_t imageHeight) const {
        // Dispatch rendering based on the type of shapes in the world
        // For simplicity, we assume all shapes are of the same type here
        if (objects.size() == 0) {
            return Image(imageWidth, imageHeight); // Return empty image if no objects
        }

        return camera.renderScene3DLight(imageWidth, imageHeight, objects, lights);
    }

} // namespace rendering
