#ifndef SHAPE_HPP
#define SHAPE_HPP

// Geometry Imports
#include "../Geometry/Box.h"
#include "../Geometry/Circle.h"
#include "../Geometry/Plane.h"
#include "../Geometry/Rectangle.h"
#include "../Geometry/Sphere.h"
#include "../Geometry/Vector3D.h"

// Rendering Imports
#include "./Material.h"

#include <memory>
#include <stdexcept>
#include <type_traits>

namespace rendering {

    /**
     * @brief Type trait to check if a type is an allowed geometry type
     */
    template<typename T>
    struct is_allowed_geometry : std::false_type {};

    // Specializations for allowed types
    template<> struct is_allowed_geometry<geometry::Box> : std::true_type {};
    template<> struct is_allowed_geometry<geometry::Circle> : std::true_type {};
    template<> struct is_allowed_geometry<geometry::Plane> : std::true_type {};
    template<> struct is_allowed_geometry<geometry::Rectangle> : std::true_type {};
    template<> struct is_allowed_geometry<geometry::Sphere> : std::true_type {};

    // Helper variable template for easier usage
    template<typename T>
    constexpr bool is_allowed_geometry_v = is_allowed_geometry<T>::value;

    /**
     * @class Shape
     * @brief Template wrapper class that combines a geometric shape with a color
     * 
     * This template class holds a geometric shape object (like Box, Circle, etc.)
     * along with an RGBA color for rendering. The geometric shape is referenced
     * via a pointer for polymorphism and memory management.
     * 
     * @tparam GeometryType The type of geometric shape - RESTRICTED to:
     *         geometry::Box, geometry::Circle, geometry::Plane, 
     *         geometry::Rectangle, geometry::Sphere only
     * 
     * Usage example:
     *   Shape<geometry::Box> coloredBox;        ✓ Valid
     *   Shape<geometry::Circle> coloredCircle;  ✓ Valid
     *   Shape<std::string> invalidShape;        ✗ Compile Error!
     */
    template<typename GeometryType, 
             typename = std::enable_if_t<is_allowed_geometry_v<GeometryType>>>
    class Shape {
    public:
        #pragma region Constructors

        /**
         * @brief Default constructor
         * Creates a shape with no geometry and no material
         */
        Shape() : geometry(nullptr), material(nullptr) {}

        /**
         * @brief Constructor with geometry only
         * @param geom The geometric shape to wrap
         */
        explicit Shape(const GeometryType& geom) 
            : geometry(std::make_unique<GeometryType>(geom)), material(nullptr) {}

        /**
         * @brief Constructor with geometry and color (creates material from color)
         * @param geom The geometric shape to wrap
         * @param shapeColor The RGBA color for this shape (converted to material)
         */
        Shape(const GeometryType& geom, const RGBA_Color& shapeColor) 
            : geometry(std::make_unique<GeometryType>(geom)), 
              material(std::make_unique<Material>(shapeColor)) {}

        /**
         * @brief Constructor with geometry and material
         * @param geom The geometric shape to wrap
         * @param shapeMaterial The Material for this shape
         */
        Shape(const GeometryType& geom, const Material& shapeMaterial) 
            : geometry(std::make_unique<GeometryType>(geom)), 
              material(std::make_unique<Material>(shapeMaterial)) {}

        /**
         * @brief Constructor with geometry pointer and color
         * @param geom Unique pointer to the geometric shape (takes ownership)
         * @param shapeColor The RGBA color for this shape (converted to material)
         */
        Shape(std::unique_ptr<GeometryType> geom, const RGBA_Color& shapeColor)
            : geometry(std::move(geom)), 
              material(std::make_unique<Material>(shapeColor)) {}

        /**
         * @brief Constructor with geometry pointer and material
         * @param geom Unique pointer to the geometric shape (takes ownership)
         * @param shapeMaterial The Material for this shape
         */
        Shape(std::unique_ptr<GeometryType> geom, const Material& shapeMaterial)
            : geometry(std::move(geom)), 
              material(std::make_unique<Material>(shapeMaterial)) {}

        /**
         * @brief Copy constructor
         * @param other The shape to copy from
         */
        Shape(const Shape& other) 
            : geometry(other.geometry ? std::make_unique<GeometryType>(*other.geometry) : nullptr),
              material(other.material ? std::make_unique<Material>(*other.material) : nullptr) {}

        /**
         * @brief Move constructor
         * @param other The shape to move from
         */
        Shape(Shape&& other) noexcept 
            : geometry(std::move(other.geometry)), 
              material(std::move(other.material)) {}

        /**
         * @brief Destructor (default, smart pointers handle cleanup)
         */
        ~Shape() = default;

        #pragma endregion

        #pragma region Assignment Operators

        /**
         * @brief Copy assignment operator
         * @param other The shape to copy from
         * @return Reference to this shape
         */
        Shape& operator=(const Shape& other) {
            if (this != &other) {
                geometry = other.geometry ? std::make_unique<GeometryType>(*other.geometry) : nullptr;
                material = other.material ? std::make_unique<Material>(*other.material) : nullptr;
            }
            return *this;
        }

        /**
         * @brief Move assignment operator
         * @param other The shape to move from
         * @return Reference to this shape
         */
        Shape& operator=(Shape&& other) noexcept {
            if (this != &other) {
                geometry = std::move(other.geometry);
                material = std::move(other.material);
            }
            return *this;
        }

        #pragma endregion

        #pragma region Geometry Management

        /**
         * @brief Get the geometric shape
         * @return Pointer to the geometric shape, or nullptr if no geometry is set
         */
        const GeometryType* getGeometry() const {
            return geometry.get();
        }

        /**
         * @brief Get the geometric shape (non-const version)
         * @return Pointer to the geometric shape, or nullptr if no geometry is set
         */
        GeometryType* getGeometry() {
            return geometry.get();
        }

        /**
         * @brief Set the geometric shape
         * @param geom The new geometric shape
         */
        void setGeometry(const GeometryType& geom) {
            geometry = std::make_unique<GeometryType>(geom);
        }

        /**
         * @brief Set the geometric shape with unique pointer
         * @param geom Unique pointer to the geometric shape (takes ownership)
         */
        void setGeometry(std::unique_ptr<GeometryType> geom) {
            geometry = std::move(geom);
        }

        /**
         * @brief Check if the shape has geometry assigned
         * @return True if the shape has geometry, false otherwise
         */
        bool hasGeometry() const {
            return geometry != nullptr;
        }

        /**
         * @brief Remove the geometry from the shape
         */
        void clearGeometry() {
            geometry.reset();
        }

        #pragma endregion

        #pragma region Material Management

        /**
         * @brief Get the shape's material
         * @return Pointer to the Material object, or nullptr if no material is set
         */
        const Material* getMaterial() const {
            return material.get();
        }

        /**
         * @brief Get the shape's material (non-const version)
         * @return Pointer to the Material object, or nullptr if no material is set
         */
        Material* getMaterial() {
            return material.get();
        }

        /**
         * @brief Set the shape's material
         * @param newMaterial The new Material for this shape
         */
        void setMaterial(const Material& newMaterial) {
            material = std::make_unique<Material>(newMaterial);
        }

        /**
         * @brief Check if the shape has a material assigned
         * @return True if the shape has a material, false otherwise
         */
        bool hasMaterial() const {
            return material != nullptr;
        }

        /**
         * @brief Remove the material from the shape
         */
        void clearMaterial() {
            material.reset();
        }

        // Backward compatibility methods for color access
        /**
         * @brief Get the shape's color (backward compatibility)
         * @return Pointer to the albedo color from the material, or nullptr if no material is set
         * @deprecated Use getMaterial() instead
         */
        const RGBA_Color* getColor() const {
            if (!material || !material->hasAlbedo()) {
                return nullptr;
            }
            // Return a pointer to a static color to maintain compatibility
            static RGBA_Color tempColor;
            tempColor = material->getAlbedo();
            return &tempColor;
        }

        /**
         * @brief Set the shape's color (backward compatibility)
         * @param color The RGBA color to set (creates a material with this as albedo)
         * @deprecated Use setMaterial() instead
         */
        void setColor(const RGBA_Color& color) {
            if (!material) {
                material = std::make_unique<Material>(color);
            } else {
                material->setAlbedo(color);
            }
        }

        /**
         * @brief Check if the shape has a color assigned (backward compatibility)
         * @return True if the shape has a material with albedo, false otherwise
         * @deprecated Use hasMaterial() instead
         */
        bool hasColor() const {
            return material && material->hasAlbedo();
        }

        geometry::Vector3D getNormalAt(const geometry::Vector3D& point) const {
            if (!geometry) {
                throw std::runtime_error("Shape has no geometry");
            }
            
            return geometry->getNormalAt(point);
        }

        #pragma endregion

        #pragma region Convenience Methods

        /**
         * @brief Check if the shape is complete (has both geometry and material)
         * @return True if the shape has both geometry and material, false otherwise
         */
        bool isComplete() const {
            return hasGeometry() && hasMaterial();
        }

        /**
         * @brief Check if the shape is valid
         * @return True if the shape has geometry and the geometry is valid
         */
        bool isValid() const {
            if (!hasGeometry()) {
                return false;
            }
            // If the geometry type has an isValid() method, call it
            return isValidHelper();
        }

    private:
        // SFINAE helper for checking if GeometryType has isValid() method
        template<typename T = GeometryType>
        auto isValidHelper() const -> decltype(std::declval<T*>()->isValid(), bool()) {
            return geometry->isValid();
        }
        
        // Fallback when isValid() doesn't exist
        bool isValidHelper(...) const {
            return true;
        }

    public:

        /**
         * @brief Access the geometry directly (throws if no geometry)
         * @return Reference to the geometric shape
         * @throws std::runtime_error if no geometry is set
         */
        const GeometryType& operator*() const {
            if (!geometry) {
                throw std::runtime_error("Shape has no geometry");
            }
            return *geometry;
        }

        /**
         * @brief Access the geometry directly (throws if no geometry)
         * @return Reference to the geometric shape
         * @throws std::runtime_error if no geometry is set
         */
        GeometryType& operator*() {
            if (!geometry) {
                throw std::runtime_error("Shape has no geometry");
            }
            return *geometry;
        }

        /**
         * @brief Access the geometry methods directly
         * @return Pointer to the geometric shape
         * @throws std::runtime_error if no geometry is set
         */
        const GeometryType* operator->() const {
            if (!geometry) {
                throw std::runtime_error("Shape has no geometry");
            }
            return geometry.get();
        }

        /**
         * @brief Access the geometry methods directly
         * @return Pointer to the geometric shape
         * @throws std::runtime_error if no geometry is set
         */
        GeometryType* operator->() {
            if (!geometry) {
                throw std::runtime_error("Shape has no geometry");
            }
            return geometry.get();
        }

        #pragma endregion

    private:
        std::unique_ptr<GeometryType> geometry;  ///< The geometric shape (owned by this object)
        std::unique_ptr<Material> material;       ///< The material of the shape (owned by this object)
    };

    // Common type aliases for convenience
    using ColoredBox = Shape<::geometry::Box>;
    using ColoredCircle = Shape<::geometry::Circle>;
    using ColoredSphere = Shape<::geometry::Sphere>;
    using ColoredPlane = Shape<::geometry::Plane>;
    using ColoredRectangle = Shape<::geometry::Rectangle>;

} // namespace rendering

#endif // SHAPE_HPP
