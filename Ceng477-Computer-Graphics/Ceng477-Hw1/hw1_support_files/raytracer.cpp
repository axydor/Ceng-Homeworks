#include "raytracer.h"
#include <thread>

/////////////////////////////////HELPERS-START/////////////////////////////////

float sq_distance(const Vec3f& a, const Vec3f& b)
{
	return pow(a.x-b.x,2) + pow(a.y-b.y,2) + pow(a.z-b.z,2);
}

void clamp(parser::Vec3f& L)
{
	if(L.x > 255)
		L.x = 255;
	if(L.y > 255)
		L.y = 255;
	if(L.z > 255)
		L.z = 255;
}

void find_normal(const parser::Scene& scene, const Ray& ray, parser::Sphere& sphere)
{
    sphere.n = (ray.e + (ray.d * ray.t_min) - scene.vertex_data[sphere.center_vertex_id-1])/sphere.radius;
    //normalize(sphere.n);
}

void find_normal(parser::Face& face, const parser::Vec3f a, const parser::Vec3f b, const parser::Vec3f c)
{
	cross_product(face.n, b-a, c-a);
	normalize(face.n);
}

void find_normal(parser::Triangle& tri, const parser::Vec3f a, const parser::Vec3f b, const parser::Vec3f c)
{
    cross_product(tri.n, b-a, c-a);
    normalize(tri.n);
}

float max(const float a, const float b)
{
	return a < b ? b : a; 
}

void print_vector(const Vec3f& vec)
{
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

void cross_product(Vec3f& result, const Vec3f& a, const Vec3f& b)
{
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

float dot_product(const Vec3f& a, const Vec3f& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void normalize(Vec3f& vec)
{
	float len_sq = pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2);
	float one_over_len = 1.0f / sqrt(len_sq);
	vec = vec * one_over_len;
}

void compute_m(Vec3f& m, const parser::Camera& camera)
{
    //  m = e + -w * distance
    m = camera.position + camera.gaze * camera.near_distance;
}

void compute_q(Vec3f& q, const Vec3f& m, const Vec3f& u, const parser::Camera& camera)
{
	// near_plane x-left, y-right, z-bottom, w-top
    // q = m + u*l + v*t
    q = m + u * camera.near_plane.x + camera.up * camera.near_plane.w;
}


void set_color(unsigned char* const image, const Vec3f& color, int* i)
{
	image[(*i)++] = color.x;
	image[(*i)++] = color.y;
	image[(*i)++] = color.z;
}


// Computes the each pixel's width and height
float pixel_length(float* difference, int total)
{
    return (*difference)/total;
}

// Find the length of the borders of the image plane
void compute_near_plane(float* r_l, float* t_b, const Vec4f& near_plane)
{
    *r_l = near_plane.y - near_plane.x;
	*t_b = near_plane.w - near_plane.z;
}

float determinant(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
	// first row of the matrice is a, second row b and third row c
	// {a.x a.y a.z}
	// {b.x b.y b.z}
	// {c.x c.y c.z}
	return a.x * (b.y*c.z - b.z*c.y) + b.x *(a.z*c.y- a.y*c.z) + c.x * (a.y*b.z - b.y*a.z); 
}

float discriminant(const Ray& ray, const Vec3f& c, float d2, float e_c2, const float r, const float b)
{
    return sqrt(pow(b, 2) - d2 * e_c2 + d2 * pow(r, 2)); 
}

/////////////////////////////////HELPERS-END///////////////////////////////////

// We send ray to each object in the scene
// We find the closest object and t value for ray 
// It returns positive number If ray hits the object, 0 otherwise
int ray_to_objects(Ray& ray, parser::Scene& scene, parser::Surface* closest)
{
	// We send ray to each object in the scene
	// We add t_min-material_id values to the map named closest
	int material_id=0;
    Vec3f v = ray.d; // Normalized vector for viewer
    normalize(v);
    v = v * - 1;

	parser::Triangle* tri = new parser::Triangle();
	if(ray_to_triangles(ray, scene, tri, v)) 
	{
		material_id++;
		*closest = *tri;
	}
	delete tri;

	parser::Face* face = new parser::Face();
	if(ray_to_meshes(ray, scene, face,v)) 
	{
		material_id++;		
		*closest = *face;
	}
	delete face;

	// We setted the ray.t_min up until here
	parser::Sphere sphere;
	if(ray_to_spheres(ray, scene, sphere))
	{
		material_id++;
		*closest = sphere;
	} 

	return material_id;
}

Vec3f shading(const Ray& ray, parser::Scene& scene, parser::Surface& closest, int recursion)
{
// Unit vectors
// l: light, n: normal, v: viewer, p: point of intersection
//
	Vec3f L{0,0,0};
	if(recursion == -1)
	{
		return L;
	}
	recursion--;
	const parser::Material material = scene.materials[closest.material_id-1];
	const Vec3f point = ray.e + (ray.d * ray.t_min);
	const float epsilon = scene.shadow_ray_epsilon;
    Vec3f v, l;  // viewer and light unit vectors

    v = ray.d;
    normalize(v);
    v = v * - 1;

    // We send ray to each light source from point p
	for(const parser::PointLight light : scene.point_lights)
	{
		Ray ray_to_light;
		parser::Surface object;
		float distance2 = sq_distance(light.position, point); // square of theDistance between light source and the object

	    l = light.position - point;
	    normalize(l);

	    ray_to_light.d = l;
	    ray_to_light.e = point + (l*epsilon);

//		SHADOWING

		 // Ray hits an object before reaching the light
		if(ray_to_objects(ray_to_light, scene, &object))
		{
			Vec3f hit_point = ray_to_light.e + (ray_to_light.d * ray_to_light.t_min);
			float distance_to_object = sq_distance(hit_point, ray_to_light.e);
			if(distance_to_object < distance2)
				continue;
		}

		L += diffuse(light.intensity/distance2, material.diffuse, l, closest.n);
        L += blinn_phong(light.intensity/distance2, material.specular, l, closest.n, v, material.phong_exponent);

	}
	if( 0 <= recursion) // If check is made in order not to make the calculations below if recursion == -1 most of the time 
	{
        parser::Surface* new_closest = new parser::Surface();  // This is the closest object to be hit by the ray
        const Vec3f d = v*-1;
        Ray r;
        r.d = d - ( ( closest.n * dot_product(closest.n,d)) * 2);
        //normalize(r.d);
        r.e = point + r.d * epsilon;

        if(ray_to_objects(r, scene, new_closest))
        {
            parser::Material new_material = scene.materials[new_closest->material_id-1];
            if(new_material.mirror.x > 0 || new_material.mirror.y > 0 || new_material.mirror.z > 0)
            {
                L += shading(r, scene, *new_closest, recursion) * material.mirror; //* material.mirror
            }
            else
                L += shading(r, scene, *new_closest, 0) * material.mirror;
        }
        delete new_closest;
	}
	L += ambiance(scene.ambient_light, material.ambient);
	return L;
}

void generate_rays(parser::Scene& scene, unsigned char* image, const parser::Camera& camera, int k, int j_p, int height_param)
{
	// Variable names are taken from the slides
    int recursion_depth = 0;
	const Vec3f background_color{scene.background_color.x, scene.background_color.y, scene.background_color.z};
	float su, sv;
	float r_l, t_b;
	float r_l_nx, t_b_ny;
	Vec3f m, q, s, u, v=camera.up;
	Ray ray;

	ray.e = camera.position;
    cross_product(u, camera.up, camera.gaze);
    compute_m(m, camera);
	compute_q(q, m, u, camera);
	compute_near_plane(&r_l, &t_b, camera.near_plane);
    r_l_nx = pixel_length(&r_l, camera.image_width);
    t_b_ny = pixel_length(&t_b, camera.image_height);

	int k_p=k;
	for(int j=j_p; j < height_param; j++)
	{
		for(int i=camera.image_width-1; 0 <= i; i--)
		{
			parser::Surface* closest = new parser::Surface();  // This is the closest object to be hit by the ray
    		
    		su = r_l_nx * (i+0.5);
			sv = t_b_ny * (j+0.5);
			s = q + (u * su) - (v * sv);
			ray.d = s - camera.position;
            //normalize(ray.d);
			ray.t_min = INFINITY;

            // We found the nearest object that intersects our ray
            if(ray_to_objects(ray, scene, closest))
            {
            	recursion_depth = 0;
            	parser::Material material = scene.materials[closest->material_id-1];
				
                if(material.mirror.x > 0 || material.mirror.y > 0 || material.mirror.z > 0)
					recursion_depth = scene.max_recursion_depth;
            	
                Vec3f L = shading(ray, scene, *closest, recursion_depth);
            	clamp(L);
            	set_color(image, L, &k_p); 
            }
            else
                set_color(image, background_color, &k_p);
 
            delete closest;
		}
	}	
}	
// It returns the material id of the closest triangle
// If there is none, it returns 0
int ray_to_triangles(Ray& ray, const parser::Scene& scene, parser::Triangle* object, const Vec3f& v)
{
    int material_id = 0;

    // Send ray to each triangle in the scene
    for(parser::Triangle tri : scene.triangles)
    {
        Vec3f a = scene.vertex_data[tri.indices.v0_id - 1];
        Vec3f b = scene.vertex_data[tri.indices.v1_id - 1];
        Vec3f c = scene.vertex_data[tri.indices.v2_id - 1];

        if(ray_triangle_intersect(ray,a,b,c))
        {
            material_id = tri.material_id;
            *object = tri;
        }	
    }
    return material_id;
}

// It returns the material id of the closest triangle of the mesh
// If there is none, it returns 0
int ray_to_meshes(Ray& ray, const parser::Scene& scene, parser::Face* object, const Vec3f& v)
{
    int material_id = 0;

    // Send ray to each meshes in the scene
    for(parser::Mesh mesh : scene.meshes)
    {
        for(parser::Face face : mesh.faces)
        {
            Vec3f a = scene.vertex_data[face.v0_id - 1];
            Vec3f b = scene.vertex_data[face.v1_id - 1];
            Vec3f c = scene.vertex_data[face.v2_id - 1];

            if(ray_triangle_intersect(ray,a,b,c))
            { 
                material_id = mesh.material_id;
                object->material_id = mesh.material_id;
                object->n = face.n;
            }	
        }
    }
    return material_id;
}

// It returns the material id of the closest sphere
// If there is none, it returns 0
int ray_to_spheres(Ray& ray, parser::Scene& scene,parser::Sphere& object)
{
    int material_id = 0;
	sphere_points sp;

    // Send ray to each sphere in the scene
    for(parser::Sphere sphere : scene.spheres)
    {
        Vec3f c = scene.vertex_data[sphere.center_vertex_id - 1];
        if(ray_sphere_intersect(ray, c, sphere.radius, sp))
        { 
            find_normal(scene, ray, sphere);
            object.n = sphere.n;
            object.material_id = sphere.material_id;
            //object = sphere;
            material_id = sphere.material_id;
        }	
    }
    return material_id;
}

bool ray_sphere_intersect(Ray& ray, const Vec3f& c, const float r, sphere_points& sp)
{
	// Variable names are adopted from the book page 77
	// (p-c)(p-c)-R^2=0. We change p with r(t)
	// Check discriminant 
	const Vec3f e_c = ray.e - c;
	const float	b = dot_product(ray.d, e_c);  // b in equation -> a^2x + bx + c = 0
	const float d2 = dot_product(ray.d, ray.d); // d.d
    const float e_c2 = dot_product(e_c, e_c) ; //(e-c).(e-c)
	const float discri = discriminant(ray, c, d2, e_c2, r, b);
    
	if(discri < 0)
		return false; 
	else
	{
		float _b;
		_b = b * -1;
		sp.t1 = (_b + discri)/d2;
		sp.t2 = (_b - discri)/d2;

		if(sp.t2 > 0 && sp.t2 < ray.t_min){
			ray.t_min = sp.t2;
			return true;
		}
		else if(sp.t1 > 0 && sp.t1 < ray.t_min)
			ray.t_min = sp.t1;
        else
            return false;
		return true;
	}
}

bool ray_triangle_intersect(Ray& ray, const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
	// A matrice 
	// {a d g} 				{a.x - b.x, a.x - c.x, ray.d.x};
	// {b e h}  -- -- -- >  {a.y - b.y, a.y - c.y, ray.d.y}; 
	// {c f i}				{a.z - b.z, a.z - c.z, ray.d.z}; 
	const float epsilon = 1e-6;
    float G, B, A, t; // Gamma, Beta and Determinant of Matrice A for using Cramer's Rule
	Vec3f a1, a2, a3;
	Vec3f b1, b2, b3;
	Vec3f g1, g2, g3;
	Vec3f t1, t2, t3;

	a1 = {a.x - b.x, a.x - c.x, ray.d.x}; 
	a2 = {a.y - b.y, a.y - c.y, ray.d.y}; 
	a3 = {a.z - b.z, a.z - c.z, ray.d.z}; 

	b1 = {a.x - ray.e.x, a.x - c.x, ray.d.x}; 
	b2 = {a.y - ray.e.y, a.y - c.y, ray.d.y}; 
	b3 = {a.z - ray.e.z, a.z - c.z, ray.d.z}; 

	g1 = {a.x - b.x, a.x - ray.e.x, ray.d.x}; 
	g2 = {a.y - b.y, a.y - ray.e.y, ray.d.y}; 
	g3 = {a.z - b.z, a.z - ray.e.z, ray.d.z}; 

	t1 = {a.x - b.x, a.x - c.x, a.x - ray.e.x}; 
	t2 = {a.y - b.y, a.y - c.y, a.y - ray.e.y}; 
	t3 = {a.z - b.z, a.z - c.z, a.z - ray.e.z}; 

	A = determinant(a1,a2,a3);
	if(A == 0)
		return false;
	G = determinant(g1,g2,g3)/A;
	if(G < 0 || G > 1)
		return false;
	B = determinant(b1,b2,b3)/A;
	if(B < 0 || B + G > 1)
		return false;

	t = determinant(t1,t2,t3)/A;
	if(t < epsilon)
		return false;

	if(t > ray.t_min)
		return false;
	
	ray.t_min = t;
	return true;
}

Vec3f ambiance(const Vec3f& intensity, const Vec3f& k)
{
	return intensity * k;
}

Vec3f diffuse(const Vec3f& intensity, const Vec3f& k, const Vec3f& l, const Vec3f& n)
{
	Vec3f L;
	L = intensity * k * max(0, dot_product(l, n));
	return L;
}

Vec3f blinn_phong(const Vec3f& intensity, const Vec3f& k, const Vec3f& l, const Vec3f& n, const Vec3f& v, const float bp)
{
	Vec3f L; // Lightning
	Vec3f h; // half vector 

	h = l + v;
	normalize(h);
	L = intensity * k * pow(max(0,dot_product(n, h)), bp);

	return L;
}
