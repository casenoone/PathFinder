#ifndef MOVINGSPHERE_H
#define MOVINGSPHERE_H
#include "hittable.h"


class moving_sphere : public hittable {
public:
	moving_sphere() {}
	moving_sphere(
		point3 cen0, point3 cen1, double t0, double t1, double r, shared_ptr<material> m)
		: center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
	{};

	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
	point3 center(double time) const;
	virtual bool translation(const double &x, const double &y, const double &z) { return false; }
	virtual bool rotate() { return false; }
	virtual bool scale(const double &sx, const double &sy, const double &sz) { return false; }

public:
	point3 center0, center1;
	double time0, time1;
	double radius;
	shared_ptr<material> mat_ptr;
};

point3 moving_sphere::center(double time) const {
	return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);//返回t0到t1之间某一时刻球体球心的位置
}

bool moving_sphere::hit(
	const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius*radius;

	auto discriminant = half_b*half_b - a*c;

	if (discriminant > 0) {
		auto root = sqrt(discriminant);

		auto temp = (-half_b - root) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			auto outward_normal = (rec.p - center(r.time())) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}

		temp = (-half_b + root) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			auto outward_normal = (rec.p - center(r.time())) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const {
	aabb box0(//这里有个疑问，移动中的盒子怎么计算其包围盒？是在计算出移动球体球心时添加吗？
		center(t0) - vec3(radius, radius, radius),
		center(t0) + vec3(radius, radius, radius));
	aabb box1(
		center(t1) - vec3(radius, radius, radius),
		center(t1) + vec3(radius, radius, radius));
	output_box = surrounding_box(box0, box1);
	return true;
}

#endif 