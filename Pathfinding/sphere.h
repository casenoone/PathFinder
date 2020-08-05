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

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {   //����t_min��t_max�к��ã�
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
		temp = (-half_b + root) / a;        //����Ϊ��δ���û�ã���������ִ�е�����//�Բۣ�Ҳ���ԣ�֮ǰдjs��׷û����һ������//��Ϊ��֪�����ڱ��滹�������
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


//��������и����ʣ�u��v�ļ��㹫ʽ��ʲô��˼��phi��theta�ļ����ǲ��ǳ����ˣ�Ӧ�ò��ǣ�������ϵ��ͬ���¹�ʽ��ͬ
void get_sphere_uv(const vec3& p, double& u, double& v) {//��������Ĺ����ǰ�p���xyz����ת����uv����
	auto phi = atan2(p.z(), p.x());
	auto theta = asin(p.y());//�����Ͼ��z��y�ǲ���д���ˣ�
	u = 1 - (phi + pi) / (2 * pi);//�����ʲô��˼�������ˣ�asin��Щ�����ķ���ֵ�и�ֵ��������Ҫ��uv�ķ�Χ�ǡ�0��1��������Ҫ��ֵ��任�����������
	v = (theta + pi / 2) / pi;
}

#endif