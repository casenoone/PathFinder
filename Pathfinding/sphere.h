#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vector3.h"

class sphere : public hittable {
public:
	sphere() {}
	sphere(point3 cen, double r, shared_ptr<material> m)
		: center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {   //设置t_min和t_max有何用？
	vec3 oc = r.origin() - center;
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
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-half_b + root) / a;        //我认为这段代码没用，根本不会执行到它的//卧槽，也不对，之前写js光追没有这一步啊！//因为不知道是内表面还是外表面
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.at(rec.t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(double t0, double t1, aabb& output_box) const {
	output_box = aabb(
		center - vec3(radius, radius, radius),
		center + vec3(radius, radius, radius));

	return true;
}


//这个函数有个疑问，u与v的计算公式是什么意思？phi和theta的计算是不是出错了？应该不是，是坐标系不同导致公式不同
void get_sphere_uv(const vec3& p, double& u, double& v) {//这个函数的功能是把p点的xyz坐标转换成uv坐标
	auto phi = atan2(p.z(), p.x());
	auto theta = asin(p.y());//这句和上句的z和y是不是写反了？
	u = 1 - (phi + pi) / (2 * pi);//这个是什么意思？明白了，asin这些函数的返回值有负值，而我们要求uv的范围是【0，1】，所以要把值域变换到这个区间内
	v = (theta + pi / 2) / pi;
}

#endif