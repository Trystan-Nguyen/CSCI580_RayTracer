#ifndef MATERIAL_H
#define MATERIAL_H

struct material {
    material(const color& kd, const color& ka, const color& ks, float shininess, float kr)
        : kd(kd), ka(ka), ks(ks), shininess(shininess), kr(kr) {}

    color ka;
    color kd;
    color ks;
    float shininess;
    float kr;
};
#endif