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
		hittable_list(shared_ptr<hittable> object) { add(object); }//shared_ptr�ǹ���ָ�룬����Ϊʲôʹ�ù���ָ�룬���˿����Զ������ڴ棬��ʱ���㲻����������ã����������ᵽ�������йص����
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



//������������ã�//Ӧ������һ������������Χ��
bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const {//�����������Ŀ����������Χ�У�//ȷʵ����������������ռ��������޴�ƽ�棬����ô����//���޴�ƽ����û�а�Χ�е�
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;
	for (const auto& object : objects) {
		if (!object->bounding_box(t0, t1, temp_box)) return false;//�����и����⣬�����ǰobjectû�а�Χ�У���ô����false�������ͽ����ˣ�//Ŀǰ��˵���������л�δ�漰�����޴�ƽ�������ļ����壬�������Ҳ�����û�а�Χ�е����
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);//surronding_box()������ʲô���ǰ�output_box��temp_box�ŵ�һ������İ�Χ����
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