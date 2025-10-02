//
// Created by villerot on 02/10/25.
//

#include "RGBA_Color.h"
#include <stdexcept>
#include <algorithm>
#include <iomanip>

namespace rendering {

    // Constructors
    RGBA_Color::RGBA_Color() : math::Vector(4, 0.0) {
        // Creates a transparent black color (0, 0, 0, 0)
    }

    RGBA_Color::RGBA_Color(double r, double g, double b, double a) : math::Vector(4) {
        values[0] = r;  // Red
        values[1] = g;  // Green
        values[2] = b;  // Blue
        values[3] = a;  // Alpha
    }

    RGBA_Color::RGBA_Color(const math::Vector& v) : math::Vector(v) {
        if (v.size() != 4) {
            throw std::invalid_argument("Vector must have exactly 4 components to create an RGBA Color");
        }
    }

    // Component accessors
    double RGBA_Color::r() const {
        return at(0);
    }

    double RGBA_Color::g() const {
        return at(1);
    }

    double RGBA_Color::b() const {
        return at(2);
    }

    double RGBA_Color::a() const {
        return at(3);
    }

    // Component setters
    void RGBA_Color::setR(double red) {
        values[0] = red;
    }

    void RGBA_Color::setG(double green) {
        values[1] = green;
    }

    void RGBA_Color::setB(double blue) {
        values[2] = blue;
    }

    void RGBA_Color::setA(double alpha) {
        values[3] = alpha;
    }

    void RGBA_Color::setRGBA(double red, double green, double blue, double alpha) {
        values[0] = red;
        values[1] = green;
        values[2] = blue;
        values[3] = alpha;
    }

    // Utility methods
    RGBA_Color RGBA_Color::clamp() const {
        return RGBA_Color(
            std::clamp(r(), 0.0, 1.0),
            std::clamp(g(), 0.0, 1.0),
            std::clamp(b(), 0.0, 1.0),
            std::clamp(a(), 0.0, 1.0)
        );
    }

    RGBA_Color RGBA_Color::toGrayscale(const double& rw, const double& gw, const double& bw) const {
        // Standard luminance weights: 0.299*R + 0.587*G + 0.114*B
        double luminance = rw * r() + gw * g() + bw * b();
        return RGBA_Color(luminance, luminance, luminance, a());
    }

    // Output stream operator
    std::ostream& operator<<(std::ostream& os, const RGBA_Color& color) {
        os << std::fixed << std::setprecision(3)
           << "RGBA(" << color.r() << ", " << color.g() << ", " 
           << color.b() << ", " << color.a() << ")";
        return os;
    }

    // Convenience color functions
    namespace Colors {
        RGBA_Color black() {
            return RGBA_Color(0.0, 0.0, 0.0, 1.0);
        }

        RGBA_Color white() {
            return RGBA_Color(1.0, 1.0, 1.0, 1.0);
        }

        RGBA_Color red() {
            return RGBA_Color(1.0, 0.0, 0.0, 1.0);
        }

        RGBA_Color green() {
            return RGBA_Color(0.0, 1.0, 0.0, 1.0);
        }

        RGBA_Color blue() {
            return RGBA_Color(0.0, 0.0, 1.0, 1.0);
        }

        RGBA_Color transparent() {
            return RGBA_Color(0.0, 0.0, 0.0, 0.0);
        }
    }

}
