#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vector3.h"

class triangle :public hittable {
public:
	triangle(){}
	triangle(point3 a, point3 b, point3 c, shared_ptr<material> m)
		:P0(a), P1(b), P2(c), mat_ptr(m){};
	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
	virtual bool translation(const double &x,const double &y,const double &z);
	virtual bool rotate();
	virtual bool scale(const double &sx, const double &sy, const double &sz);


public:
	point3 P0;
	point3 P1;
	point3 P2;
	shared_ptr<material> mat_ptr;

};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

	point3 E1 = P1 - P0;
	point3 E2 = P2 - P0;
	point3 S = r.origin() - P0;
	point3 S1 = cross(r.direction(), E2);
	point3 S2 = cross(S, E1);
	
	double temp1 = 1 / (dot(S1, E1));
	double t = temp1*(dot(S2, E2));
	double b1 = temp1*(dot(S1, S));
	double b2 = temp1*(dot(S2, r.direction()));
	double b3 = 1 - b1 - b2;
	if (t >= 0 && b1 >= 0 && b2 >= 0 && b3 >= 0) {
		if (t < t_max && t > t_min) {
			rec.t = t;
			rec.p = r.at(rec.t);
			vec3 outward_normal = unit_vector(cross(E1,E2));
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}

bool triangle::bounding_box(double t0, double t1, aabb& output_box) const {//三角形包围盒怎么求？或者说，单个三角形根本没有包围盒？//求三角形外接圆，进而求该圆所在的球，再求该球的包围盒

	double a1, b1, c1, d1;
	double a2, b2, c2, d2;
	double a3, b3, c3, d3;

	double x1 = P0.x(), y1 = P0.y(), z1 = P0.z();
	double x2 = P1.x(), y2 = P1.y(), z2 = P1.z();
	double x3 = P2.x(), y3 = P2.y(), z3 = P2.z();

	a1 = (y1*z2 - y2*z1 - y1*z3 + y3*z1 + y2*z3 - y3*z2);
	b1 = -(x1*z2 - x2*z1 - x1*z3 + x3*z1 + x2*z3 - x3*z2);
	c1 = (x1*y2 - x2*y1 - x1*y3 + x3*y1 + x2*y3 - x3*y2);
	d1 = -(x1*y2*z3 - x1*y3*z2 - x2*y1*z3 + x2*y3*z1 + x3*y1*z2 - x3*y2*z1);

	a2 = 2 * (x2 - x1);
	b2 = 2 * (y2 - y1);
	c2 = 2 * (z2 - z1);
	d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

	a3 = 2 * (x3 - x1);
	b3 = 2 * (y3 - y1);
	c3 = 2 * (z3 - z1);
	d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

	double x4 = -(b1*c2*d3 - b1*c3*d2 - b2*c1*d3 + b2*c3*d1 + b3*c1*d2 - b3*c2*d1)
		/ (a1*b2*c3 - a1*b3*c2 - a2*b1*c3 + a2*b3*c1 + a3*b1*c2 - a3*b2*c1);
	double y4 = (a1*c2*d3 - a1*c3*d2 - a2*c1*d3 + a2*c3*d1 + a3*c1*d2 - a3*c2*d1)
		/ (a1*b2*c3 - a1*b3*c2 - a2*b1*c3 + a2*b3*c1 + a3*b1*c2 - a3*b2*c1);
	double z4 = -(a1*b2*d3 - a1*b3*d2 - a2*b1*d3 + a2*b3*d1 + a3*b1*d2 - a3*b2*d1)
		/ (a1*b2*c3 - a1*b3*c2 - a2*b1*c3 + a2*b3*c1 + a3*b1*c2 - a3*b2*c1);

	double radius = sqrt((x4 - x1)*(x4 - x1) + (y4 - y1)*(y4 - y1)*(z4 - z1)*(z4 - z1));

	vec3 center(x4, y4, z4);

	output_box = aabb(
		center - vec3(radius, radius, radius),
		center + vec3(radius, radius, radius)
	);
	

	return true;
}

bool triangle::translation(const double &x,const double &y,const double &z) {
	P0.translate_vector(x, y, z);
	P1.translate_vector(x, y, z);
	P2.translate_vector(x, y, z);

	return true;
}

bool triangle::rotate() {
	return false;
}

bool triangle::scale(const double &sx, const double &sy, const double &sz) {
	P0.scale_vector(sx, sy, sz);
	P1.scale_vector(sx, sy, sz);
	P2.scale_vector(sx, sy, sz);

	return true;
}

#endif