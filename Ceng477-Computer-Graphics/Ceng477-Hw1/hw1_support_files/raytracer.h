#include <iostream>
#include <math.h>
#include <map>
#include "parser.h"

#ifndef __RAY_TRACER__
#define __RAY_TRACER__


typedef parser::Vec3f Vec3f;
typedef parser::Vec4f Vec4f;
typedef parser::Vec3i Vec3i;
typedef unsigned char RGB[3];


typedef struct Ray {
	Vec3f e; // origin of the ray
	Vec3f d; // direction of the ray
	float t_min = INFINITY;
} Ray;

typedef struct sphere_points {
	float t1;
	float t2;
}sphere_points;

void find_normal(parser::Triangle& tri, const parser::Vec3f a, const parser::Vec3f b, const parser::Vec3f c);
void find_normal(parser::Face& face, const parser::Vec3f a, const parser::Vec3f b, const parser::Vec3f c);
void find_normal(const parser::Scene& scene, const Ray& ray, const parser::Sphere& sphere);
float max(const float a, const float b);
void print_vector(const Vec3f& vec);
void cross_product(Vec3f& result,const Vec3f& a, const Vec3f& b);
float dot_product(const Vec3f& a, const Vec3f& b);
void normalize(Vec3f& vec);
void compute_m(Vec3f& m, const parser::Camera& camera);
void compute_q(Vec3f& q, const Vec3f& m, const Vec3f& u, const parser::Camera& camera);
void set_color(unsigned char* image, const Vec3f& L, int& i);
Vec3f shading(const Ray& ray, parser::Scene& scene, parser::Surface& closest, int recursion);
void generate_rays(parser::Scene& scene, unsigned char* image, const parser::Camera& camera, int k, int j, int height_param);
bool ray_sphere_intersect(Ray& ray, const Vec3f& c, const float r, sphere_points& sp);
bool ray_triangle_intersect( Ray& ray, const Vec3f& a, const Vec3f& b, const Vec3f& c);
float determinant(const Vec3f& a, const Vec3f& b, const Vec3f& c);
float pixel_length(float* difference, int total);
int ray_to_objects(Ray& ray, parser::Scene& scene, parser::Surface* closest);
void compute_near_plane(float* r_l, float* t_b, const Vec4f& near_plane);
int ray_to_triangles( Ray& ray, const parser::Scene& scene, parser::Triangle* object, const Vec3f& v);
int ray_to_spheres( Ray& ray, parser::Scene& scene, parser::Sphere& object);
int ray_to_meshes( Ray& ray, const parser::Scene& scene, parser::Face* object, const Vec3f& v);
float discriminant(const Vec3f& ray, const Vec3f& c, float d2, float e_c2, const float r, const float b);
Vec3f ambiance(const Vec3f& intensity, const Vec3f& k);
Vec3f diffuse(const Vec3f& intensity, const Vec3f& k, const Vec3f& wi, const Vec3f& n);
Vec3f blinn_phong(const Vec3f& intensity, const Vec3f& k, const Vec3f& l, const Vec3f& n, const Vec3f& v, const float bp);

#endif
