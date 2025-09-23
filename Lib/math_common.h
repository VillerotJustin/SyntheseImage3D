//
// Created by villerot on 17/03/25.
//

#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include <cmath>

namespace math {
    // Defining constants
    constexpr double pi = 3.14159265358979323846;

    inline double square(const double x) { return x * x; }

    inline double triangle_area(const double a, const double b, const double c) {
        double semi_perimeter = (a + b + c) * 0.5;
        auto f = [=](double s) {return semi_perimeter - s;};
        return std::sqrt(semi_perimeter * f(a) * f(b) * f(c));
    }

}


#endif //MATH_COMMON_H
