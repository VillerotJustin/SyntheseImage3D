//
// Created by villerot on 03/10/2025.
//

#ifndef WORLD_H
#define WORLD_H

#include "../Geometry/Vector3D.h"
#include "../Geometry/Quaternion.h"
#include "../Geometry/Ray.h"
#include "../Geometry/Rectangle.h"
#include "../Geometry/Box.h"
#include "../Geometry/Circle.h"
#include "../Geometry/Plane.h"
#include "../Geometry/Sphere.h"

#include "../Math/Vector.hpp"

#include "./Image.h"
#include "./Shape.hpp"
#include "./Camera.h"
#include "./Light.h"

#include <variant>
#include <algorithm>

namespace rendering {

    class World {
    public:  
        World();

        /**
         * Add an object to the world
         * @tparam T The geometry type of the shape to add
         * @param shape The shape to add
         */
        template<typename T>
        void addObject(const Shape<T>& shape);

        /**
         * Remove an object from the world
         * @param index The index of the shape to remove
         */
        void removeObjectAt(const size_t index);

        /**
         * Add a light to the world
         * @param light The light to add
         */
        void addLight(const Light& light);

        /**
         * Remove a light from the world
         * @param light The light to remove
         */
        void removeLight(const Light& light);

        /**
         * Remove a light from the world
         * @param index The index of the light to remove
         */
        void removeLightAt(const size_t& index);

        /**
         * Get the number of objects in the world
         * @return The number of objects
         */
        size_t getObjectCount() const;

        /**
         * Get the camera
         * @return Reference to the camera
         */
        Camera& getCamera();
        const Camera& getCamera() const;

        /**
         * Clear all objects from the world
         */
        void clearObjects();

        /**
         * Render the scene from the camera's perspective
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @return Image The rendered image
         */
        Image renderScene2DColor(size_t imageWidth, size_t imageHeight) const;

        /**
         * Render the scene to a depth map from the camera's perspective
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @return Image The rendered depth map image
         */
        Image renderScene2DDepth(size_t imageWidth, size_t imageHeight) const;

        /**
         * Render the scene with just default color
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @return Image The rendered depth map image
         */
        Image renderScene3DColor(size_t imageWidth, size_t imageHeight) const;

        /**
         * Render the scene with depth lighting
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @return Image The rendered depth map image
         */
        Image renderScene3DDepth(size_t imageWidth, size_t imageHeight) const;

        /**
         * Render the scene with light lighting
         * @param imageWidth The width of the output image in pixels
         * @param imageHeight The height of the output image in pixels
         * @return Image The rendered depth map image
         */
        Image renderScene3DLight(size_t imageWidth, size_t imageHeight) const;


    private:
        using ShapeVariant = std::variant<Shape<geometry::Box>, Shape<geometry::Circle>, Shape<geometry::Plane>, Shape<geometry::Rectangle>, Shape<geometry::Sphere>>;
        math::Vector<ShapeVariant> objects;

        math::Vector<Light> lights;

        Camera camera;
    };

} // namespace rendering

// Template method implementations
namespace rendering {

    template<typename T>
    void World::addObject(const Shape<T>& shape) {
        ShapeVariant* shapeVariant = new ShapeVariant{shape};
        objects.append(shapeVariant);
    }

}

#endif // WORLD_H