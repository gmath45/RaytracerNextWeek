#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "texture.h"

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(Point3(0,0,0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    
    cam.vfov              =             20;
    cam.lookfrom          = Point3(0,0,12);
    cam.lookat            = Point3(0,0, 0);
    cam.vup               =   Vec3(0,1, 0);

    cam.defocus_angle = 0;
    
    cam.render(hittable_list(globe), "earth");
}

int main() {
    earth();
}