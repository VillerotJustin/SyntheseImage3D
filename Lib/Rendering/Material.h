//
// Created by villerot on 06/11/2025.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "./RGBA_Color.h"

#include <stdexcept>
#include <memory>


namespace rendering {

    class Material {
        public: 

        
        private:
            std::unique_ptr<RGBA_Color> albedo; // The base color of the material
            std::unique_ptr<RGBA_Color> specular; // The specular color of the material tint reflections
            std::unique_ptr<RGBA_Color> emissive; // The emissive color of the material

            double absorption = 0.0; // The absorption coefficient of the material (0.0 = no absorption, 1.0 = full absorption)
            double roughness = 0.5; // The roughness of the material surface (0.0 = smooth, 1.0 = rough)
            double metalness = 0.0; // The metalness of the material (0.0 = non-metal, 1.0 = metal)

    };

}

#endif // MATERIAL_H