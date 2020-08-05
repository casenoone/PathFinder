#ifndef BVH_H
#define BVH_H
#include <algorithm>
#include "hittable_list.h"
#include<iostream>
class bvh_node : public hittable {
public:
	bvh_node();

	bvh_node(hittable_list& list, double time0, double time1)//ί�й��캯����//�����listָ��������İ�Χ�У���Ȼ����ͬ��list����в�ͬ��objects��
		: bvh_node(list.objects, 0, list.objects.size(), time0, time1)//time0��time1��������ʲô������moving_sphere���õ�
	{}

	bvh_node(
		std::vector<shared_ptr<hittable>>& objects,//����ΪʲôҪ��std::?
		size_t start, size_t end, double time0, double time1);
	//start����
	//end����
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

bool bvh_node::bounding_box(double t0, double t1, aabb& output_box) const {//���������ʲô��
	output_box = box;
	return true;
}

//hit���������ã��ڲ�ԭ��û�и㶮�����Ҳ���ϸ�ڣ�ֻҪ֪�����������������������r���ĸ������ཻ
bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	if (!box.hit(r, t_min, t_max))//���û�����к��ӣ���������
		return false;

	bool hit_left = left->hit(r, t_min, t_max, rec);//��������ˣ���ô�Ϳ�����������û������ //ʵ������һ���������
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);//hit_left ? rec.t : t_max���û����

	return hit_left || hit_right;
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {//���������ʲô���ã�
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min().e[axis] < box_b.min().e[axis];//�Ƚ�˭�İ�Χ����axis�����ϸ���ǰ
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

bvh_node::bvh_node(//bvh_node�Ĺ��캯��
	std::vector<shared_ptr<hittable>>& objects,
	size_t start, size_t end, double time0, double time1//start��end�Ǹ�ʲô�ģ��Ҳ������ǿ�ʼ���������������±ꣿ//start=0,end=sizeof(objects)
) {                                                     
	int axis = random_double(0, 2);//���ѡ��һ����
	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare
		: box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {//���ѡ��1����ô����y���������Ƚ�
		left = right = objects[start];//���ӵ����Һ��ӵ��ڿ�ʼ�Ǹ�����
	}
	else if (object_span == 2) {//���ѡ��2����ô����z���������Ƚ�
		if (comparator(objects[start], objects[start + 1])) {//Ϊ��comparator���Դ���������Ϊcomparator�õ�һ������ָ��
			left = objects[start];             //�����z�����ϣ�
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);//�������x���϶�������������

		auto mid = start + object_span / 2;
		//�ѵ�������һ���ݹ麯����������<bvh_node>�ڴ�ʱ����һ��ִ��bvh_node�Ĺ��캯������ȷ������...
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);//�����ڸ�ʲô������������
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);//Ϊʲôleft���Ժ�make_shared<bvh_node>���Ⱥţ�
		//left��hittable���͵Ĺ���ָ��
		
	}

	aabb box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left)//Ϊ�˷�ֹ�������޴��ƽ��
		|| !right->bounding_box(time0, time1, box_right)
		)
		std::cerr << "No bounding box in bvh_node constructor.\n";

	box = surrounding_box(box_left, box_right);
	
}


#endif