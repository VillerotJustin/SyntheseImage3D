//
// Created by villerot on 02/10/25.
//

#include "RGBA_Color.h"
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <cmath>

namespace rendering {

    // Constructors
    RGBA_Color::RGBA_Color() : components(4, 0.0) {
        // Creates a transparent black color (0, 0, 0, 0)
    }

    RGBA_Color::RGBA_Color(double r, double g, double b, double a) : components({r, g, b, a}) {
    }

    RGBA_Color::RGBA_Color(const math::Vector& v) : components(v) {
        if (v.size() != 4) {
            throw std::invalid_argument("Vector must have exactly 4 components to create an RGBA Color");
        }
    }

    // Component accessors
    double RGBA_Color::r() const {
        return components.at(0);
    }

    double RGBA_Color::g() const {
        return components.at(1);
    }

    double RGBA_Color::b() const {
        return components.at(2);
    }

    double RGBA_Color::a() const {
        return components.at(3);
    }

    // Component setters
    void RGBA_Color::setR(double red) {
        components = math::Vector({red, g(), b(), a()});
    }

    void RGBA_Color::setG(double green) {
        components = math::Vector({r(), green, b(), a()});
    }

    void RGBA_Color::setB(double blue) {
        components = math::Vector({r(), g(), blue, a()});
    }

    void RGBA_Color::setA(double alpha) {
        components = math::Vector({r(), g(), b(), alpha});
    }

    void RGBA_Color::setRGBA(double red, double green, double blue, double alpha) {
        components = math::Vector({red, green, blue, alpha});
    }

    // Color-specific operations
    RGBA_Color RGBA_Color::operator+(const RGBA_Color& other) const {
        math::Vector result = components + other.components;
        return RGBA_Color(result);
    }

    RGBA_Color RGBA_Color::operator-(const RGBA_Color& other) const {
        math::Vector result = components - other.components;
        return RGBA_Color(result);
    }

    RGBA_Color RGBA_Color::operator*(double scalar) const {
        math::Vector result = components * scalar;
        return RGBA_Color(result);
    }

    RGBA_Color RGBA_Color::operator*(const RGBA_Color& other) const {
        return RGBA_Color(
            r() * other.r(),
            g() * other.g(),
            b() * other.b(),
            a() * other.a()
        );
    }

    bool RGBA_Color::operator==(const RGBA_Color& other) const {
        return components == other.components;
    }

    bool RGBA_Color::operator!=(const RGBA_Color& other) const {
        return !(components == other.components);
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

    RGBA_Color RGBA_Color::lerp(const RGBA_Color& other, double t) const {
        t = std::clamp(t, 0.0, 1.0);
        return RGBA_Color(
            r() + t * (other.r() - r()),
            g() + t * (other.g() - g()),
            b() + t * (other.b() - b()),
            a() + t * (other.a() - a())
        );
    }

    RGBA_Color RGBA_Color::alphaBlend(const RGBA_Color& background) const {
        double alpha = a();
        double invAlpha = 1.0 - alpha;
        
        return RGBA_Color(
            alpha * r() + invAlpha * background.r(),
            alpha * g() + invAlpha * background.g(),
            alpha * b() + invAlpha * background.b(),
            alpha + invAlpha * background.a()
        );
    }

    const math::Vector& RGBA_Color::asVector() const {
        return components;
    }

    // Output stream operator
    std::ostream& operator<<(std::ostream& os, const RGBA_Color& color) {
        os << std::fixed << std::setprecision(3)
           << "RGBA(" << color.r() << ", " << color.g() << ", " 
           << color.b() << ", " << color.a() << ")";
        return os;
    }

    // Friend scalar multiplication
    RGBA_Color operator*(double scalar, const RGBA_Color& color) {
        return color * scalar;
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
