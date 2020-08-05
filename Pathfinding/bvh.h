#ifndef BVH_H
#define BVH_H
#include <algorithm>
#include "hittable_list.h"
#include<iostream>
class bvh_node : public hittable {
public:
	bvh_node();

	bvh_node(hittable_list& list, double time0, double time1)//委托构造函数？//这里的list指的是物体的包围盒？显然，不同的list里就有不同的objects，
		: bvh_node(list.objects, 0, list.objects.size(), time0, time1)//time0和time1的作用是什么？可能moving_sphere会用到
	{}

	bvh_node(
		std::vector<shared_ptr<hittable>>& objects,//这里为什么要用std::?
		size_t start, size_t end, double time0, double time1);
	//start：？
	//end：？
	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

	virtual bool translation(const double &x, const double &y, const double &z) { return false; }
	virtual bool rotate() { return false; }
	virtual bool scale(const double &sx, const double &sy, const double &sz) { return false; }

public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
};

bool bvh_node::bounding_box(double t0, double t1, aabb& output_box) const {//这个函数有什么用
	output_box = box;
	return true;
}

//hit函数的作用？内部原理还没有搞懂，暂且不管细节，只要知道，这个函数是用来检测光线r与哪个物体相交
bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	if (!box.hit(r, t_min, t_max))//如果没有射中盒子，则函数结束
		return false;

	bool hit_left = left->hit(r, t_min, t_max, rec);//如果射中了，那么就看其左子树有没有射中 //实际上是一个先序遍历
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);//hit_left ? rec.t : t_max这个没看懂

	return hit_left || hit_right;
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {//这个函数是什么作用？
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min().e[axis] < box_b.min().e[axis];//比较谁的包围盒在axis方向上更靠前
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 2);
}

bvh_node::bvh_node(//bvh_node的构造函数
	std::vector<shared_ptr<hittable>>& objects,
	size_t start, size_t end, double time0, double time1//start与end是干什么的？我猜想它是开始物体与结束物体的下标？//start=0,end=sizeof(objects)
) {                                                     
	int axis = random_double(0, 2);//随机选择一个轴
	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare
		: box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {//如果选到1，那么就在y方向上做比较
		left = right = objects[start];//左孩子等于右孩子等于开始那个物体
	}
	else if (object_span == 2) {//如果选到2，那么就在z方向上作比较
		if (comparator(objects[start], objects[start + 1])) {//为甚comparator可以传参数？因为comparator得到一个函数指针
			left = objects[start];             //如果在z方向上，
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);//否则就在x轴上对所有物体排序

		auto mid = start + object_span / 2;
		//难道这里是一个递归函数？当申请<bvh_node>内存时，又一次执行bvh_node的构造函数？的确是这样...
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);//这是在干什么？在生成树吗？
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);//为什么left可以和make_shared<bvh_node>划等号？
		//left是hittable类型的共享指针
		
	}

	aabb box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left)//为了防止输入无限大的平面
		|| !right->bounding_box(time0, time1, box_right)
		)
		std::cerr << "No bounding box in bvh_node constructor.\n";

	box = surrounding_box(box_left, box_right);
	
}


#endif