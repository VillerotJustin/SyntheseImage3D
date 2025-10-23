//
// Created by villerot on 23/10/2024.
//

#include "Light.h"

namespace rendering {

    Light::Light(const Vector3D& position, const RGBA_Color& color, double intensity) {
        this->position = position;
        this->color = color;
        this->intensity = std::max(0.0, std::min(1.0, intensity));
    }

    void Light::translate(const Vector3D& translation) {
        position = position + translation;
    }

} // namespace rendering