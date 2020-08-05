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
		hittable_list(shared_ptr<hittable> object) { add(object); }//shared_ptr�ǹ���ָ�룬����Ϊʲôʹ�ù���ָ�룬��ʱ���㲻�������д���һ�ܵĳ������������Щ����
		void clear() { objects.clear(); }
		void add(shared_ptr<hittable> object) { objects.push_back(object); }

		virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
		virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
	public:
		std::vector<shared_ptr<hittable>> objects;//����һ��shared_ptr<hittable>���͵�����objects������
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;
	//�����const auto&�Ǵ�ָ�������ֵ���ǵģ�����������ã���ôobjects���Ԫ�ص�ֵ����ı䣬ֻ���βη����˸ı�
	for (const auto& object : objects) {//����ʲôд����//��ʾ����ȡ��objects�����е�Ԫ��//����objectsΪʲô������auto���ͣ�//��Ϊauto�������Ը��ݱ���ֵ�Զ�ƥ�����ͣ������ˣ�
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

#endif