#ifndef TEXTURE_H
#define TEXTURE_H
#include "rtweekend.h"
#include "perlin.h"
#include "rtw_stb_image.h"
#include <iostream>

class texture {
public:
	virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
	solid_color() {}
	solid_color(color c) : color_value(c) {}

	solid_color(double red, double green, double blue)//构造函数后面接冒号，再接构造函数，啥意思？
		: solid_color(color(red, green, blue)) {}//这里第一个solid_color是重载函数，第二个solid_texture是调用solid_color(color c)这个函数

	virtual color value(double u, double v, const vec3& p) const {
		return color_value;
	}
	
private:
	color color_value;
};

class checker_texture : public texture {
public:
	checker_texture() {}
	checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1) : even(t0), odd(t1) {}

	virtual color value(double u, double v, const point3& p) const {
		auto sines = sin(10 * p.x())*sin(10 * p.y())*sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);//在checktexture
		else
			return even->value(u, v, p);
	}

public:
	shared_ptr<texture> odd;
	shared_ptr<texture> even;
};


class noise_texture : public texture {
public:
	noise_texture() {}
	noise_texture(double sc) : scale(sc) {}
	virtual color value(double u, double v, const point3& p) const {
		return color(1, 1, 1) * 0.5 * (1 + sin(scale*p.z() + 10 * noise.turb(p)));
	}

public:
	perlin noise;
	double scale;
};

//图片纹理这块基本没看懂 
class image_texture : public texture {
public:
	const static int bytes_per_pixel = 3;//这是什么？

	image_texture()
		: data(nullptr), width(0), height(0), bytes_per_scanline(0) {}//这些参数是什么意思

	image_texture(const char* filename) {
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load(
			filename, &width, &height, &components_per_pixel, components_per_pixel);//第三方库函数

		if (!data) {
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = 0;
		}

		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture() {
		delete data;
	}

	virtual color value(double u, double v, const vec3& p) const {
		// If we have no texture data, then return solid cyan as a debugging aid.
		if (data == nullptr)
			return color(0, 1, 1);

		// Clamp input texture coordinates to [0,1] x [1,0]
		u = clamp(u, 0.0, 1.0);        //这两段代码啥意思？坐标系转换？应该是坐标系转换
		v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

		auto i = static_cast<int>(u * width);//这又是在干什么？//
		auto j = static_cast<int>(v * height);

		// Clamp integer mapping, since actual coordinates should be less than 1.0
		if (i >= width)  i = width - 1;    //这又是在干什么？
		if (j >= height) j = height - 1;

		const auto color_scale = 1.0 / 255.0;
		auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;//这又是在干什么？

		return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
	}

private:
	unsigned char *data;
	int width, height;
	int bytes_per_scanline;
};


#endif