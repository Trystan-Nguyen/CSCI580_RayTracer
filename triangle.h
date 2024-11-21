#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
public:
    triangle(const point3& v0, const point3& v1, const point3& v2) : v0(v0), v1(v1), v2(v2) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
		vec3 n = unit_vector(cross(v1 - v0, v2 - v0));

		auto LeftEq_t = dot(n, r.direction());
		auto RightEq = -dot(n, (r.origin() - v0));

		if (LeftEq_t == 0) return false;

		float t = RightEq / LeftEq_t;
		if (t < 0.0001) return false;
		
		auto p = r.at(t);
		
		// Cross products
		vec3 cross0 = cross(v1 - v0, p - v0); 
		vec3 cross1 = cross(v2 - v1, p - v1);  
		vec3 cross2 = cross(v0 - v2, p - v2);

		// Check if the cross products have the same direction
		if ((dot(cross0, cross1) >= 0 && dot(cross1, cross2) >= 0 && dot(cross2, cross0) >= 0)) {
			rec.t = t;
			rec.p = p;
			rec.normal = n;
			return true;
		}
		else return false;
		return true;
    }

private:
    point3 v0;
    point3 v1;
    point3 v2;
};

#endif