#ifndef CAMERA_H
#define CAMERA_H

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"
#include "hittable.h"
#include "material.h"
#include <vector>

class camera {
    public:
        /* Public Camera Parameters */
        double  aspect_ratio      = 1.0;    // Image ratio width/height
        int     image_width       = 100;    // Redered width in Pixels
        int     samples_per_pixel = 10;     // # of random samples
        int     max_depth         = 10;     // Max # of Ray Bounces

        double vfov     = 90;               // Vertical Field-of-View
        Point3 lookfrom = Point3(0,0,0);    // Point camera is looking from
        Point3 lookat   = Point3(0,0,-1);   // Point camera is lookin at
        Vec3   vup      = Vec3(0,1,0);      // Camera-relative "up"

        double defocus_angle =  0;          // Variation angle of rays through each pixel
        double focus_dist    = 10;          // Distance from camera `lookfrom` Point to Plane of perfect focus

        void render(const hittable& world, const std::string& source_file) {
            initialize();

            std::vector<unsigned char> buffer(image_width * image_height * 3);

            for (int j = 0; j < image_height; j++) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    color pixel_color(0,0,0);
                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    int index = (j * image_width + i) * 3;      // Adding index for PNG pixel data
                    write_color_to_buffer(buffer.data(), index, pixel_samples_scale * pixel_color); // PNG pixel data
                }
            }
            std::string output_path = "img/" + source_file + ".png";
            stbi_write_png(output_path.c_str(), image_width, image_height, 3, buffer.data(), image_width * 3);    // PNG creation
            std::clog << "\rDone.                 \n";
        }

    private:
        /* Private Camera Parameters */
        int    image_height;         // Rendered image height
        double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
        Point3 center;               // Camera center
        Point3 pixel00_loc;          // Location of pixel 0, 0
        Vec3   pixel_delta_u;        // Offset to pixel to the right
        Vec3   pixel_delta_v;        // Offset to pixel below
        Vec3   u, v, w;              // Camera frame basis vectors
        Vec3   defocus_disk_u;       // Defocus disk Horizontal radius
        Vec3   defocus_disk_v;       // Defocus disk Vertical radius

        void initialize() {
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            center = lookfrom;

            // Determine viewport specs
            auto theta = degrees_to_radians(vfov);              // Theta for fov
            auto h = std::tan(theta/2);                         // tan(theta) for height
            auto viewport_height = 2 * h * focus_dist;        // set height
            auto viewport_width = viewport_height * (double(image_width)/image_height);

            // Calculate u, v, w Unit basis Vectors
            w = UV(lookfrom - lookat);
            u = UV(cross(vup, w));
            v = cross(w, u);

            // Calculate Vectors along Viewport edges
            auto viewport_u = viewport_width * u;       // Horizontal edge Vector
            auto viewport_v = viewport_height * -v;     // Vertical edge Vector

            // Calculate Pixel-to-Pixel Vectors
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate Origin(Upper Left)
            auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
            
            // Calculate camera defocus
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(int i, int j) const {
            // Construct Camera ray from Defocus Disk -> Random Points around pixel location i,j
            auto offset = sample_square();
            auto pixel_sample = pixel00_loc +
                                ((i + offset.x()) * pixel_delta_u) +
                                ((j + offset.y()) * pixel_delta_v);
            
            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;
            auto ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        Vec3 sample_square() const {
            // Return Vector @ points [-0.5,-0.5]->[0.5,0.5]
            return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        Point3 defocus_disk_sample() const {
            // Returns random point in the camera defocus disk
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            // If Ray Bounce limit reached, no more light
            if (depth <= 0) return color(0,0,0);

            hit_record rec;

            if (world.hit(r, interval(0.001, INF), rec)) {          // 0.001 allows for tolerance in floating-point rounding errors
                ray scattered;
                color attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered)) return attenuation * ray_color(scattered, depth-1, world);
                return color(0,0,0);
            }

            Vec3 unit_direction = UV(r.direction());
            auto a = 0.5 * (unit_direction.y() + 1.0);
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
        }

};

#endif