#include "./Sphere.h"
#include "../Math/math_common.h"

namespace geometry {

    // Constructor
    Sphere::Sphere(const Vector3D& center, double radius)
        : center(center), radius(radius) {
        // Ensure radius is positive for a valid sphere
        if (radius <= 0) {
            throw std::invalid_argument("Radius must be positive");
        }
    }

    // Accessors

    double Sphere::getSurfaceArea() const {
        return 4 * math::pi * radius * radius;
    }

    double Sphere::getVolume() const {
        return (4.0 / 3.0) * math::pi * radius * radius * radius;
    }

    // Core Geometry Methods

    bool Sphere::containsPoint(const Vector3D& point) const {
        double distSquared = (point - center).lengthSquared();
        return distSquared < radius * radius;
    }

    bool Sphere::surfacePoint(const Vector3D& point) const {
        double distSquared = (point - center).lengthSquared();
        return std::abs(distSquared - radius * radius) <= 1e-9;
    }

    double Sphere::distanceToPoint(const Vector3D& point) const {
        return (point - center).length() - radius;
    }

    Vector3D Sphere::closestPointOnSurface(const Vector3D& point) const {
        Vector3D dir = (point - center).normal();
        return center + dir * radius;
    }

    // Intersections

    bool Sphere::intersects(const Sphere& other) const {
        double distBetweenSpheres = (other.center - center).length();
        double radiusSum = this->radius + other.radius;
        double radiusDiff = std::abs(this->radius - other.radius);
        
        // Spheres intersect if they are not too far apart AND one is not inside the other
        return distBetweenSpheres <= radiusSum && distBetweenSpheres >= radiusDiff;
    }

    std::optional<std::variant<Circle, Vector3D, Sphere>> Sphere::intersectionPoints(const Sphere& other) const {
        double distBetweenSpheres = (other.center - center).length();
        if (distBetweenSpheres > radius + other.radius || distBetweenSpheres < std::abs(radius - other.radius)) {
            return std::nullopt; // No intersection too far apart or one inside the other
        }

        // Case of identical spheres
        if (*this == other) {
            return *this; // Infinite intersection points (the spheres are identical)
        }

        // Case of tangent spheres with some precision tolerance
        if (std::abs(distBetweenSpheres - (radius + other.radius)) <= 1e-9 || // Externally tangent
            std::abs(distBetweenSpheres - std::abs(radius - other.radius)) <= 1e-9) { // Internally tangent
            Vector3D point = center + (other.center - center).normal() * radius;
            return point; // Single intersection point
        }

        /*
            Case of intersecting spheres
            Sphere 1          Intersection Circle          Sphere 2
            •────────────────────○────────────────────────•
            C1        a          P                        C2
            ↑                    ↑                        ↑
            Center 1         Circle Center              Center 2
                            
            |←────── radius ──────→|
                                |←─── other.radius ───→|
            |←────────── distBetweenSpheres ─────────→|

            Mathematical Derivation:
            This formula comes from solving the triangle formed by:

            Center of sphere 1 (C1)
            Center of sphere 2 (C2)
            Center of intersection circle (P)
            Using the law of cosines or Pythagorean theorem:

            Distance from C1 to P = a
            Distance from C2 to P = distBetweenSpheres - a
            Distance from P to intersection circle edge = h (height, calculated next)
            The formula ensures that:
                radius² = a² + h²                    (Pythagorean theorem for sphere 1)
                other.radius² = (d - a)² + h²        (Pythagorean theorem for sphere 2)
        */
        
        double a = (radius * radius - other.radius * other.radius + distBetweenSpheres * distBetweenSpheres) / (2 * distBetweenSpheres);
        double circleRadius = std::sqrt(radius * radius - a * a); // or h

        Vector3D circleCenter = center + (other.center - center).normal() * a; // Or P
        Vector3D circleNormal = (other.center - center).normal();

        return Circle(circleCenter, circleRadius, circleNormal); // Radius will be set later
    }

    bool Sphere::rayIntersect(const Ray& ray, double t) const {
        // Vector from ray origin to sphere center
        Vector3D oc = ray.getOrigin() - center;
        double distancefromCenterToRayOrigin = oc.length();

        // if the ray origin is inside the sphere
        if (oc.lengthSquared() < radius * radius) {
            return true;
        }

        // if ray point at sphere center
        if (oc.parallel(ray.getDirection())) {
            return true;
        }

        for (double progress = 0; progress <= distancefromCenterToRayOrigin; progress += t) {
            Vector3D pointOnRay = ray.getPointAt(progress);
            if (containsPoint(pointOnRay)) {
                return true;
            }
        }
        return false;
    }

    double Sphere::rayIntersectionDistance(const Ray& ray, double t) const {
        // Vector from ray origin to sphere center
        Vector3D oc = ray.getOrigin() - center;
        double distancefromCenterToRayOrigin = oc.length();

        // if the ray origin is inside the sphere
        if (oc.lengthSquared() < radius * radius) {
            return 0.0;
        }

        // if ray point at sphere center
        if (oc.parallel(ray.getDirection())) {
            return distancefromCenterToRayOrigin - radius;
        }

        for (double progress = 0; progress <= distancefromCenterToRayOrigin; progress += t) {
            Vector3D pointOnRay = ray.getPointAt(progress);
            if (containsPoint(pointOnRay)) {
                return progress;
            }
        }
        return -1.0; // No intersection
    }



    bool Sphere::lineIntersects(const Vector3D& linePoint, const Vector3D& lineDirection) const {
        Vector3D oc = linePoint - center;
        double a = lineDirection.dot(lineDirection);
        double b = 2.0 * oc.dot(lineDirection);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - 4 * a * c;
        return (discriminant >= 0);
    }

    // Utility Methods

    void Sphere::translate(const Vector3D& translation) {
        center = center + translation;
    }

    bool Sphere::isValid() const {
        return radius > 0;
    }

    void Sphere::scale(double factor) {
        if (factor > 0) {
            radius *= factor;
        }
    }

    Box Sphere::getBoundingBox() const {
        Vector3D minCorner = center - Vector3D(radius, radius, radius);
        Vector3D boxNormal(1, 0, 0); // Arbitrary normals
        return Box(minCorner, 2 * radius, 2 * radius, 2 * radius, boxNormal);
    }

    Vector3D Sphere::randomPointOnSurface() const {
        double u = static_cast<double>(rand()) / RAND_MAX;
        double v = static_cast<double>(rand()) / RAND_MAX;
        double theta = 2 * math::pi * u;
        double phi = std::acos(2 * v - 1);
        double x = radius * std::sin(phi) * std::cos(theta);
        double y = radius * std::sin(phi) * std::sin(theta);
        double z = radius * std::cos(phi);
        return center + Vector3D(x, y, z);
    }

    Vector3D Sphere::randomPointInside() const {
        // Generate a random point uniformly distributed inside the sphere
        // Using rejection sampling method for uniformity
        double u, v, w, norm;
        do {
            u = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;  // [-1, 1]
            v = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;  // [-1, 1]
            w = 2.0 * static_cast<double>(rand()) / RAND_MAX - 1.0;  // [-1, 1]
            norm = u*u + v*v + w*w;
        } while (norm > 1.0);  // Reject points outside unit sphere
        
        // Scale to desired radius with uniform distribution
        double scale = radius * std::cbrt(static_cast<double>(rand()) / RAND_MAX);
        double factor = scale / std::sqrt(norm);
        
        return center + Vector3D(u * factor, v * factor, w * factor);
    }

    // Advanced Methods

    Vector3D Sphere::normalAt(const Vector3D& point) const {
        if (!surfacePoint(point)) {
            throw std::invalid_argument("Point is not on the surface of the sphere");
        }
        return (point - center).normal();
    }

    Plane Sphere::tangentPlaneAtPoint(const Vector3D& point) const {
        Vector3D normal = this->normalAt(point);
        return Plane(point, normal);
    }

    Vector3D Sphere::projectPointOntoSurface(const Vector3D& point) const {
        Vector3D dir = (point - center).normal();
        return center + dir * radius;
    }

    // Comparison Operators
    bool Sphere::operator==(const Sphere& other) const {
        return center == other.center && radius == other.radius;
    }

    bool Sphere::operator!=(const Sphere& other) const {
        return !(*this == other);
    }

    bool Sphere::equals(const Sphere& other, double epsilon) const {
        return (center - other.center).length() <= epsilon && std::abs(radius - other.radius) <= epsilon;
    }

    // ToString
    std::ostream& operator<<(std::ostream& os, const Sphere& s) {
        os << "Sphere(Center: " << s.center << ", Radius: " << s.radius << ")";
        return os;
    }

    

} // namespace geometry

