#include "math_common.h"
#include <algorithm>
#include <cmath>

namespace math {

    bool solveQuadratic(const double &a, const double &b, const double &c, double &x0, double &x1) {
        double discr = b * b - 4 * a * c;
        if (discr < 0) return false;
        else if (discr == 0) x0 = x1 = - 0.5 * b / a;
        else {
            double q = (b > 0) ?
                -0.5 * (b + std::sqrt(discr)) :
                -0.5 * (b - std::sqrt(discr));
            x0 = q / a;
            x1 = c / q;
        }
        if (x0 > x1) std::swap(x0, x1);
        return true;
    }
}
