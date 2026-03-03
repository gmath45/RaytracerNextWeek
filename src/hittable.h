#ifndef HITTABLE_H
#define HITTABLE_H

class material;

class hit_record {
    public:
        Point3 p;
        Vec3 normal;
        shared_ptr<material> mat;
        double t;
        bool front_face;
        
        // Determine if surface is front-facing
        void set_face_normal(const ray& r, const Vec3& outward_normal) {
            // NOTE: `outward_bound` assumed to have Unit Length
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class hittable {
    public:
        virtual ~hittable() = default;

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif