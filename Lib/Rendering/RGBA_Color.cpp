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
    RGBA_Color::RGBA_Color() : components(4) {
        // Creates a transparent black color (0, 0, 0, 0)
        components[0] = new double(0.0);
        components[1] = new double(0.0);
        components[2] = new double(0.0);
        components[3] = new double(0.0);
    }

    RGBA_Color::RGBA_Color(double r, double g, double b, double a) : components(4) {
        components[0] = new double(std::clamp(r, 0.0, 1.0));
        components[1] = new double(std::clamp(g, 0.0, 1.0));
        components[2] = new double(std::clamp(b, 0.0, 1.0));
        components[3] = new double(std::clamp(a, 0.0, 1.0));
    }

    RGBA_Color::RGBA_Color(const math::Vector<double>& v) : components(4) {
        if (v.size() != 4) {
            throw std::invalid_argument("Vector must have exactly 4 components to create an RGBA Color");
        }
        components[0] = new double(std::clamp(*v[0], 0.0, 1.0));
        components[1] = new double(std::clamp(*v[1], 0.0, 1.0));
        components[2] = new double(std::clamp(*v[2], 0.0, 1.0));
        components[3] = new double(std::clamp(*v[3], 0.0, 1.0));
    }

    RGBA_Color::~RGBA_Color() {
        delete components[0];
        delete components[1];
        delete components[2];
        delete components[3];
    }

    RGBA_Color::RGBA_Color(const RGBA_Color& other) : components(4) {
        components[0] = new double(*other.components[0]);
        components[1] = new double(*other.components[1]);
        components[2] = new double(*other.components[2]);
        components[3] = new double(*other.components[3]);
    }

    RGBA_Color& RGBA_Color::operator=(const RGBA_Color& other) {
        if (this != &other) {
            delete components[0];
            delete components[1];
            delete components[2];
            delete components[3];
            
            components[0] = new double(*other.components[0]);
            components[1] = new double(*other.components[1]);
            components[2] = new double(*other.components[2]);
            components[3] = new double(*other.components[3]);
        }
        return *this;
    }

    // Component accessors
    double RGBA_Color::r() const {
        return *components[0];
    }

    double RGBA_Color::g() const {
        return *components[1];
    }

    double RGBA_Color::b() const {
        return *components[2];
    }

    double RGBA_Color::a() const {
        return *components[3];
    }

    // Component setters
    void RGBA_Color::setR(double red) {
        delete components[0];
        components[0] = new double(std::clamp(red, 0.0, 1.0));
    }

    void RGBA_Color::setG(double green) {
        delete components[1];
        components[1] = new double(std::clamp(green, 0.0, 1.0));
    }

    void RGBA_Color::setB(double blue) {
        delete components[2];
        components[2] = new double(std::clamp(blue, 0.0, 1.0));
    }

    void RGBA_Color::setA(double alpha) {
        delete components[3];
        components[3] = new double(std::clamp(alpha, 0.0, 1.0));
    }

    void RGBA_Color::setRGBA(double red, double green, double blue, double alpha) {
        delete components[0];
        delete components[1];
        delete components[2];
        delete components[3];
        components[0] = new double(std::clamp(red, 0.0, 1.0));
        components[1] = new double(std::clamp(green, 0.0, 1.0));
        components[2] = new double(std::clamp(blue, 0.0, 1.0));
        components[3] = new double(std::clamp(alpha, 0.0, 1.0));
    }

    void RGBA_Color::invert() {
        setRGBA(1.0 - r(), 1.0 - g(), 1.0 - b(), a());
    }

    // Color-specific operations
    RGBA_Color RGBA_Color::operator+(const RGBA_Color& other) const {
        return RGBA_Color(
            *components[0] + *other.components[0],
            *components[1] + *other.components[1],
            *components[2] + *other.components[2],
            *components[3] + *other.components[3]
        );
    }

    RGBA_Color RGBA_Color::operator-(const RGBA_Color& other) const {
        return RGBA_Color(
            *components[0] - *other.components[0],
            *components[1] - *other.components[1],
            *components[2] - *other.components[2],
            *components[3] - *other.components[3]
        );
    }

    RGBA_Color RGBA_Color::operator*(double scalar) const {
        return RGBA_Color(
            *components[0] * scalar,
            *components[1] * scalar,
            *components[2] * scalar,
            *components[3] * scalar
        );
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
        return r() == other.r() && 
               g() == other.g() && 
               b() == other.b() && 
               a() == other.a();
    }

    bool RGBA_Color::operator!=(const RGBA_Color& other) const {
        return !(*this == other);
    }

    void RGBA_Color::clampself() {
        setRGBA(
            std::clamp(r(), 0.0, 1.0),
            std::clamp(g(), 0.0, 1.0),
            std::clamp(b(), 0.0, 1.0),
            std::clamp(a(), 0.0, 1.0)
        );
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

    const math::Vector<double>& RGBA_Color::asVector() const {
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
