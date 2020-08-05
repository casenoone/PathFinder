#include<iostream>
#include<fstream>
#include "color.h"
#include "ray.h"
#include "rtweekend.h"
#include "vector3.h"

#include "hittable_list.h"
#include "sphere.h"
#include"triangle.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "rtw_stb_image.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include "pdf.h"
#include"external\OBJ_Loader.h"
using namespace std;

objl::Loader Loader;

bool read_obj(objl::Loader Loader,hittable_list &box,string &filename, shared_ptr<material> m) {

	bool loadout = Loader.LoadFile("Lemonade_Can.obj");
	if (!loadout) {

		return false;
	}

	for (int i = 0; i < Loader.LoadedMeshes.size(); i++) {
		objl::Mesh curMesh = Loader.LoadedMeshes[i];
		
		for (int j = 0; j < curMesh.Indices.size(); j += 3)
		{
			int a, b, c;
			a = curMesh.Indices[j];
			b = curMesh.Indices[j + 1];
			c = curMesh.Indices[j + 2];
			point3 A = point3(curMesh.Vertices[a].Position.X, curMesh.Vertices[a].Position.Y, curMesh.Vertices[a].Position.Z);
			point3 B = point3(curMesh.Vertices[b].Position.X, curMesh.Vertices[b].Position.Y, curMesh.Vertices[b].Position.Z);
			point3 C = point3(curMesh.Vertices[c].Position.X, curMesh.Vertices[c].Position.Y, curMesh.Vertices[c].Position.Z);
			A.constrain_scale_vector(1500);
			B.constrain_scale_vector(1500);
			C.constrain_scale_vector(1500);
			
			A.translate_vector(200, 0, 200);
			B.translate_vector(200, 0, 200);
			C.translate_vector(200, 0, 200);
			shared_ptr<hittable> box1 = make_shared<triangle>(A, B, C, m);
			
			box.add(box1);
		}
		
		
	}
	
}

color ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable> lights, int depth) {
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	ray scattered;
	color attenuation;
	color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
	double pdf_val;
	color albedo;
	if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf_val))
		return emitted;

	shared_ptr<hittable> light_shape =
		make_shared<xz_rect>(213, 343, 227, 332, 554, make_shared<material>());
	auto p0 = make_shared<hittable_pdf>(light_shape, rec.p);
	auto p1 = make_shared<cosine_pdf>(rec.normal);
	mixture_pdf p(p0, p1);

	scattered = ray(rec.p, p.generate(), r.time());
	pdf_val = p.value(scattered.direction());

	return emitted
		+ albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_color(scattered, background, world, lights, depth - 1)
		/ pdf_val;
}



hittable_list earth() {
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	return hittable_list(globe);
}



hittable_list cornell_box(camera& cam, double aspect) {//思考一个问题，为什么说light是双面的？
	hittable_list world;

	auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
	auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
	auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
	auto blue = make_shared<lambertian>(make_shared<solid_color>(.55, .93, .93));
	auto light = make_shared<diffuse_light>(make_shared<solid_color>(50, 50, 50));
	
	//思考一下，flip_face与yz_rect的区别//flip是翻转的意思，就是添加一个与原来面法线相反的面
	world.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	world.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	world.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
	//world.add(make_shared<flip_face>(make_shared<xz_circle>(point3(277.5,554,277.5),70,light)));
	
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	world.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	world.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	//康奈尔盒子里除去长方体共有6个面，上面代码也添加了6个面，那么为什么说light是双面的呢

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	//world.add(box1);
	
	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	//world.add(box2);

	auto pertext = make_shared<noise_texture>(0.1);
	//world.add(make_shared<sphere>(point3(165, 200, 165), 80, make_shared<lambertian>(pertext)));

	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(200, 100, 165), 60, blue);
	//world.add(globe);

	hittable_list boxes1;
	//boxes1.add(globe);

	auto tri = make_shared<triangle>(point3(50, 200, 300), point3(100, 100, 300), point3(100, 50, 300), white);
	//tri = make_shared<translate>(tri, vec3(265, 0, 295));

	//boxes1.add(tri);
	//world.add(tri);
	//world.add(make_shared<bvh_node>(boxes1, 0, 1));
	string filename = "box_stack.obj";
	read_obj(Loader, boxes1, filename, white);
	//cout << boxes1.objects.size();

	world.add(make_shared<bvh_node>(boxes1, 0, 1));
	

	point3 lookfrom(278, 278, -800);
	point3 lookat(278, 278, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 40.0;
	auto t0 = 0.0;
	auto t1 = 1.0;

	cam = camera(lookfrom, lookat, vup, vfov, aspect, aperture, dist_to_focus, t0, t1);

	return world;
}


int main() {
	
	const auto aspect_ratio = 16.0 / 9.0;   
	const int image_width = 512;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 10000;
	const int max_depth = 50;

	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;//焦距

	auto origin = point3(0, 0, 0);//相机位置
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);//这里没看懂//明白了，在空间中画个图就知道了，这个向量是穿过左下角第一个像素的向量
		
	hittable_list world;
	point3 lookfrom(278, 278, -800);
	point3 lookat(278, 278, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 40.0;

	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

	auto R = cos(pi / 4);
	
	shared_ptr<hittable> lights;
	world = cornell_box(cam, aspect_ratio);
	ofstream outfile("C:\\Users\\WORLD TPO\\Desktop\\movingSphere.ppm", ios::trunc);
	if (!outfile.is_open()) {
		cout << "fail to open file";
	}


	outfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	const color background(0, 0, 0);
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;  //这个是干啥用的？
		for (int i = 0; i < image_width; ++i) {
			
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {  //反走样
				auto u = (i + random_double()) / (image_width - 1);//这里的原理是什么？就是对于单个像素，多采样一些该光线周围的点
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, background, world,lights, max_depth);

			}

			//由于暂时不清楚oufile的机理，下面代码是将writhe_color函数拆开以后的，学完原理之后再修改这些细节
			
			auto R = pixel_color.x();
			auto G = pixel_color.y();
			auto B = pixel_color.z();

			// Divide the color total by the number of samples.
			// Divide the color total by the number of samples and gamma-correct for gamma=2.0.
			auto scale = 1.0 / samples_per_pixel;
			R = sqrt(scale * R); //这里开平方是用到了伽马矫正
			G = sqrt(scale * G);
			B = sqrt(scale * B);
			
			outfile << static_cast<int>(256 * clamp(R, 0.0, 0.999)) << ' '
				<< static_cast<int>(256 * clamp(G, 0.0, 0.999)) << ' '
				<< static_cast<int>(256 * clamp(B, 0.0, 0.999)) << '\n';
		}
	}
	std::cerr << "\nDone.\n";
	outfile.close();
	return 0;
}