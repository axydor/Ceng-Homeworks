#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "raytracer.h"
#include <thread>

int main(int argc, char* argv[])
{
    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    for(parser::Camera camera : scene.cameras)
    {
        int j=0;
        int k=0;
        const int width = camera.image_width, height = camera.image_height;
        const int height_param = height / 8;
        char* file_name = &camera.image_name[0u];
        unsigned char* image = new unsigned char [width * height * 3];    
        
        std::thread t1(generate_rays, std::ref(scene), image, camera, k, j, height_param);
        j += height_param;
        k = 3 * j * width; 
        std::thread t2(generate_rays, std::ref(scene), image, camera, k, j, height_param*2);
        j += height_param;
        k = 3 * j * width; 
        std::thread t3(generate_rays, std::ref(scene), image, camera, k, j, height_param*3);
        j += height_param;
        k = 3 * j * width; 
        std::thread t4(generate_rays, std::ref(scene), image, camera, k, j, height_param*4);
        j += height_param;
        k = 3 * j * width; 
        std::thread t5(generate_rays, std::ref(scene), image, camera, k, j, height_param*5);
        j += height_param;
        k = 3 * j * width; 
        std::thread t6(generate_rays, std::ref(scene), image, camera, k, j, height_param*6);
        j += height_param;
        k = 3 * j * width; 
        std::thread t7(generate_rays, std::ref(scene), image, camera, k, j, height_param*7);
        j += height_param;
        k = 3 * j * width; 
        generate_rays(scene, image, camera, k, j, height);
        //std::thread t8(generate_rays, std::ref(scene), image, camera, k, j, height);
      
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        t6.join();
        t7.join();
        //t8.join();

        write_ppm(file_name, image, width, height);
        delete[] image;
    }
}
