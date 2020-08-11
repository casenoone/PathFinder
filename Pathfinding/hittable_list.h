#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>
#include <iostream>
using std::shared_ptr;
using std::make_shared;

class hittable_list: public hittable {
	public:
		hittable_list() {}
		hittable_list(shared_ptr<hittable> object) { add(object); }//shared_ptr是共享指针，至于为什么使用共享指针，除了可以自动回收内存，暂时还搞不清楚其他作用，例如书中提到与纹理有关的情况
		void clear() { objects.clear(); }
		void add(shared_ptr<hittable> object) { objects.push_back(object); }

		virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
		virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
		
		virtual bool translation(const double &x, const double &y, const double &z);
		virtual bool rotate();
		virtual bool scale(const double &sx, const double &sy, const double &sz);
public:
		std::vector<shared_ptr<hittable>> objects;
};


bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;
	
	for (const auto& object : objects) {
		if (object->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
			random_double();
			
		}
	}
	
	return hit_anything;
}



//这个函数的作用？//应该是求一堆物体的整体包围盒
bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const {//所以这个函数目的是求最大包围盒？//确实是这样，但是如果空间中有无限大平面，该怎么处理？//无限大平面是没有包围盒的
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;
	for (const auto& object : objects) {
		if (!object->bounding_box(t0, t1, temp_box)) return false;//这里有个问题，如果当前object没有包围盒，那么返回false，函数就结束了？//目前来说，本程序中还未涉及到无限大平面这样的几何体，所以暂且不考虑没有包围盒的情况
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);//surronding_box()作用是什么？是把output_box和temp_box放到一个更大的包围盒里
		first_box = false;
	}
	
	return true;
}

bool hittable_list::translation(const double &x, const double &y, const double &z) {
	for (auto iter = objects.begin(); iter != objects.end(); iter++) {
		(*iter)->translation(x,y,z);
	}
	return true;
}

bool hittable_list::scale(const double &sx, const double &sy, const double &sz) {
	for (auto iter = objects.begin(); iter != objects.end(); iter++) {
		(*iter)->scale(sx, sy, sz);
	}
	return true;
}

bool hittable_list::rotate() {
	return false;
}

#endif