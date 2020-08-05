#ifndef BOX_H
#define BOX_H

#include "hittable_list.h"
#include "aarect.h"


class box : public hittable {
public:
	box() {}
	box(const point3& p0, const point3& p1, shared_ptr<material> ptr);

	virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;

	virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
		output_box = aabb(box_min, box_max);
		return true;
	}

	virtual bool translation(const double &x, const double &y, const double &z) { return false; }
	virtual bool rotate() { return false; }
	virtual bool scale(const double &sx, const double &sy, const double &sz) { return false; }

public:
	point3 box_min;
	point3 box_max;
	hittable_list sides;
};

box::box(const point3& p0, const point3& p1, shared_ptr<material> ptr) {
	box_min = p0;
	box_max = p1;

	sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
	sides.add(make_shared<flip_face>(
		make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr)));

	sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
	sides.add(make_shared<flip_face>(
		make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)));

	sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
	sides.add(make_shared<flip_face>(
		make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr)));
}

bool box::hit(const ray& r, double t0, double t1, hit_record& rec) const {
	return sides.hit(r, t0, t1, rec);
}

#endif