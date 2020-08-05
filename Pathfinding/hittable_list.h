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
		hittable_list(shared_ptr<hittable> object) { add(object); }//shared_ptr是共享指针，至于为什么使用共享指针，暂时还搞不清楚，等写完第一周的程序后集中讨论这些问题
		void clear() { objects.clear(); }
		void add(shared_ptr<hittable> object) { objects.push_back(object); }

		virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
		virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
	public:
		std::vector<shared_ptr<hittable>> objects;//构建一个shared_ptr<hittable>类型的名叫objects的容器
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;
	//这里的const auto&是传指针的引用值吗？是的，如果不传引用，那么objects里的元素的值不会改变，只是形参发生了改变
	for (const auto& object : objects) {//这是什么写法？//表示依次取出objects容器中的元素//但是objects为什么可以是auto类型？//因为auto变量可以根据变量值自动匹配类型，明白了！
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

#endif