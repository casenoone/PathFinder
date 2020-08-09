#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable_list.h"
#include "texture.h"
#include "onb.h"

class material {
public:
	virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v,
		const point3& p) const {
		return color(0, 0, 0);
	}
	
	
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& albedo, ray& scattered, double& pdf
	) const {
		return false;
	}

	virtual double scattering_pdf(
		const ray& r_in, const hit_record& rec, const ray& scattered
	) const {
		return 0;
	}
	

};
//rec：交点信息记录
//attenuation：理解为自身的颜色
//scattered：散射后光线的方向


class lambertian : public material {
public:
	lambertian(shared_ptr<texture> a) : albedo(a) {}


	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& alb, ray& scattered, double& pdf
	) const {
		onb uvw;//ONB还没有搞懂
		uvw.build_from_w(rec.normal);
		auto direction = uvw.local(random_cosine_direction());
		scattered = ray(rec.p, unit_vector(direction), r_in.time());
		alb = albedo->value(rec.u, rec.v, rec.p);
		//pdf = dot(uvw.w(), scattered.direction()) / pi;
		return true;
	}

	double scattering_pdf(//其实就是BRDF
		const ray& r_in, const hit_record& rec, const ray& scattered
	) const {
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 : cosine / pi;
	}
public:
	shared_ptr<texture> albedo;
};

class metal : public material {
public:
	metal(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& alb, ray& scattered, double& pdf
	) const {
		
		onb uvw;
		uvw.build_from_w(rec.normal);
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected);
		alb = albedo->value(rec.u, rec.v, rec.p);
		pdf = dot(uvw.w(), scattered.direction()) / pi;
		return true;
	}

	double scattering_pdf(//其实就是BRDF
		const ray& r_in, const hit_record& rec, const ray& scattered
	) const {
		auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
		return cosine < 0 ? 0 :1;
	}

public:
	shared_ptr<texture> albedo;
};


/*class metal : public material {
public:
	metal(const color& a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

public:
	color albedo;
};*/

double schlick(double cosine, double ref_idx) {
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}


/*class dielectric : public material {
public:
	dielectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const {
		attenuation = color(1.0, 1.0, 1.0);
		double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
		if (etai_over_etat * sin_theta > 1.0) {
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}

		double reflect_prob = schlick(cos_theta, etai_over_etat);
		if (random_double() < reflect_prob)
		{
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}

		vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		scattered = ray(rec.p, refracted);
		return true;
	}

public:
	double ref_idx;
};
*/


class dielectric : public material {
public:
	dielectric(double ri) : ref_idx(ri) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const {
		attenuation = color(1.0, 1.0, 1.0);//纯白
		
		//
		double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
		if (etai_over_etat * sin_theta > 1.0) {
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}

		double reflect_prob = schlick(cos_theta, etai_over_etat);
		if (random_double() < reflect_prob)
		{
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}

		vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		scattered = ray(rec.p, refracted);
		return true;
	}

public:
	double ref_idx;
};



class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const {
		return false;
	}

	/*virtual color emitted(double u, double v, const point3& p) const {
		return emit->value(u, v, p);
	}*/

	virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v,
		const point3& p) const override {   //这里为什么要用override
        
		if (rec.front_face)//如果是front面，则返回自发光，否则返回0
			return emit->value(u, v, p);//这个怎么返回自发光的？
		else
			return color(0, 0, 0);
	}


public:
	shared_ptr<texture> emit;
};



class isotropic : public material {
public:
	isotropic(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const {
		scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

public:
	shared_ptr<texture> albedo;
};


#endif
