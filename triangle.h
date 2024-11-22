#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"

class triangle : public hittable {
public:
    triangle(const point3& v0, const point3& v1, const point3& v2, const vec3& n0, const vec3& n1, const vec3& n2, shared_ptr<material> mat)
		: v0(v0), v1(v1), v2(v2), n0(n0), n1(n1), n2(n2), mat(mat) {}

	vec3 interpolateVector3(vec3 t) const
	{
		float a_weight = cross(v1 - t, v2 - t).length() / (1 + (v0[2] / (MAXINT - v0[2])));
		float b_weight = cross(v0 - t, v2 - t).length() / (1 + (v1[2] / (MAXINT - v1[2])));
		float c_weight = cross(v0 - t, v1 - t).length() / (1 + (v2[2] / (MAXINT - v2[2])));
		
		float total_weight = a_weight + b_weight + c_weight;

		return n0 * (a_weight / total_weight) + n1 * (b_weight / total_weight) + n2 * (c_weight / total_weight);
	}

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
		if (!(dot(cross0, cross1) >= 0 && dot(cross1, cross2) >= 0 && dot(cross2, cross0) >= 0)) {
			return false;
		}

		if (t <= ray_tmin || ray_tmax <= t) {
			return false;
		}

		rec.t = t;
		rec.p = p;
		rec.normal = interpolateVector3(p);
		rec.mat = mat;
		return true;
    }

private:
    point3 v0;
    point3 v1;
    point3 v2;
	vec3 n0;
	vec3 n1;
	vec3 n2;
	shared_ptr<material> mat;
};

#endif