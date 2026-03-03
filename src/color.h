#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

using color = Vec3;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) return std::sqrt(linear_component);
    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply linear-to-gamma transformation
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate range [0,1] -> range [0,255]
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write pixel colors
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

// Implementing PNG creation 
inline void write_color_to_buffer(unsigned char* buffer, int index, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.000, 0.999);
    buffer[index + 0] = static_cast<unsigned char>(256 * intensity.clamp(r));
    buffer[index + 1] = static_cast<unsigned char>(256 * intensity.clamp(g));
    buffer[index + 2] = static_cast<unsigned char>(256 * intensity.clamp(b));
}

#endif
