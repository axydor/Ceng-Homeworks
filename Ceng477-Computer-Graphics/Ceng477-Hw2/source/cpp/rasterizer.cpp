#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include "hw2_types.h"
#include "hw2_math_ops.h"
#include "hw2_file_ops.h"
#include <iostream>


Camera cameras[100];
int numberOfCameras = 0;

Model models[1000];
int numberOfModels = 0;

Color colors[100000];
int numberOfColors = 0;

Translation translations[1000];
int numberOfTranslations = 0;

Rotation rotations[1000];
int numberOfRotations = 0;

Scaling scalings[1000];
int numberOfScalings = 0;

Vec3 vertices[100000];
int numberOfVertices = 0;

Color backgroundColor;

// backface culling setting, default disabled
int backfaceCullingSetting = 0;

Color **image;



/*
	Initializes image with background color
*/
void initializeImage(Camera cam) {
    int i, j;

    for (i = 0; i < cam.sizeX; i++)
        for (j = 0; j < cam.sizeY; j++) {
            image[i][j].r = backgroundColor.r;
            image[i][j].g = backgroundColor.g;
            image[i][j].b = backgroundColor.b;

        }
}

/*
	Transformations, culling, rasterization are done here.
	You can define helper functions inside this file (rasterizer.cpp) only.
	Using types in "hw2_types.h" and functions in "hw2_math_ops.cpp" will speed you up while working.
*/

#include <vector>
// If transformation is apllied to vertice
// Vertice at index with id-1 is set to True
std::vector<bool> transformed(100000,false);

Vec3 backUp[100000];

// Camera Transformation Matrix
double CAM[4][4];

// Perspective Projection Matrix
// Convert it into ortographic and then to Canonical Viewing Volume
double PER[4][4];

// ViewPort Transformation Matrix
double VIP[4][4];

void scaleVec(Vec3& vec, double scale)
{
    vec.x = vec.x * scale;
    vec.y = vec.y * scale;
    vec.z = vec.z * scale;
}

void normalize(Vec3& vec)
{
	double len_sq = pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2);
	double one_over_len = 1.0f / sqrt(len_sq);
	scaleVec(vec, one_over_len);
}

void findNormal(const Triangle& tri, Vec3& normal)
{
    Vec3 a = vertices[tri.vertexIds[0]];
    Vec3 b = vertices[tri.vertexIds[1]];
    Vec3 c = vertices[tri.vertexIds[2]];
  
    normal = crossProductVec3(subtractVec3(b,a),subtractVec3(c,a));
    normalize(normal);
}

Vec3 vectorize_u(const Rotation& rot)
{
    return Vec3{rot.ux, rot.uy, rot.uz};
}

// Set the value at index to 0 and swap other two while negating one of them
void swap(Vec3& vec, int index)
{
    double temp;
    switch(index) {
        case 0:
            vec.x = 0;
            temp = vec.y;
            vec.y = -vec.z;
            vec.z = temp;
            break;
        case 1:
            vec.y = 0;
            temp = vec.x;
            vec.x = -vec.z;
            vec.z = temp;
            break;
        case 2:
            vec.z = 0;
            temp = vec.x;
            vec.x = -vec.y;
            vec.y = temp;
            break;
    }
}

template<class T>
T min(T a, T b)
{
    return a > b ? b : a;
}

template<class T>
T min(T a, T b, T c)
{
    if(a <= min(b,c))
        return a;
    if( b <= min(a,c))
        return b;
    if( c <= min(a,b))
        return c;
    else
        return -1;
}

template<class T>
T max(T a, T b)
{
    return a < b ? b : a;
}

template<class T>
T max(T a, T b, T c)
{
    if(a >= max(b,c))
        return a;
    if( b >= max(a,c))
        return b;
    if( c >= max(a,b))
        return c;
}

// Find the minimum ABS value in wrt to 0
int findMinIndex(const Vec3& u)
{
    double x = abs(u.x);
    double y = abs(u.y);
    double z = abs(u.z);
    
    if( x <= min(y, z) )
        return 0;
    if( y <= min(x, z) )
        return 1;
    if( z <= min(x, y) )
        return 2;
    return -1;
}

void find_v(const Vec3& u, Vec3& v)
{
    int index = findMinIndex(u);
    if(index == -1)
        std::cout << "ERROR index is -1 " << std::endl;

    v.x = u.x;
    v.y = u.y;
    v.z = u.z;
    swap(v, index);
}

// It creates a translation matrix(T) to multiply it with vertice
void makeTranslationMatrix(double T[4][4], const Translation& trans)
{
    makeIdentityMatrix(T);
    T[0][3] = trans.tx;
    T[1][3] = trans.ty;
    T[2][3] = trans.tz;
}

void constructRX(double R[4][4], double cosine, double sine)
{
    makeIdentityMatrix(R);
    R[1][1] = cosine;
    R[1][2] = -sine;
    R[2][1] = sine;
    R[2][2] = cosine;
}

void constructM(double M[4][4], const Vec3& u, const Vec3& v, const Vec3& w)
{
    makeIdentityMatrix(M);
    M[0][0] = u.x;
    M[0][1] = u.y;
    M[0][2] = u.z;
    
    M[1][0] = v.x;
    M[1][1] = v.y;
    M[1][2] = v.z;
    
    M[2][0] = w.x;
    M[2][1] = w.y;
    M[2][2] = w.z;
}

void constructI_M(double I_M[4][4], const Vec3& u, const Vec3& v, const Vec3& w)
{
    makeIdentityMatrix(I_M);
    I_M[0][0] = u.x;
    I_M[0][1] = v.x;
    I_M[0][2] = w.x;
    
    I_M[1][0] = u.y;
    I_M[1][1] = v.y;
    I_M[1][2] = w.y;
    
    I_M[2][0] = u.z;
    I_M[2][1] = v.z;
    I_M[2][2] = w.z;
}

// Result Matrix is ready to multiply with vertice
// Alternative Method i.e Orhonormal Matrix is used at the Slides 
void makeRotationMatrix(double R[4][4], const Rotation& rot)
{
    // Final Rotation Transformation Matrix: R = M^-1.RX.M
    double RX[4][4];
    double M[4][4];
    double I_M[4][4];                               // Inverse of M
    double temp[4][4];
    
    double angle = rot.angle * M_PI / 180.0; 
    double cosine = cos(angle); 
    double sine = sin(angle); 

    Vec3 v;
    Vec3 u = vectorize_u(rot);
    normalize(u);
    find_v(u, v);
    normalize(v);
    Vec3 w = crossProductVec3(u,v);
    
    constructRX(RX, cosine, sine);
    constructM(M, u, v, w);
    constructI_M(I_M, u, v, w);

    multiplyMatrixWithMatrix(temp, RX, M);
    multiplyMatrixWithMatrix(R, I_M, temp);
}

void makeScaleMatrix(double S[4][4], const Scaling& sca)
{
    makeIdentityMatrix(S);
    S[0][0] = sca.sx;
    S[1][1] = sca.sy;
    S[2][2] = sca.sz;
}

////////////////////////MODELING-TRANSFORMATIONS////////////////////////////////

void scale(const Scaling& sca, double v[4], double result[4])
{
    double S[4][4];
    makeScaleMatrix(S, sca);
    multiplyMatrixWithVec4d(result, S, v);
}

void rotate(const Rotation& rot, double v[4], double result[4])
{
    double R[4][4];
    makeRotationMatrix(R, rot);
    multiplyMatrixWithVec4d(result, R, v);
}

void translate(const Translation& trans, double v[4], double result[4]) 
{
    double T[4][4];
    makeTranslationMatrix(T, trans);
    multiplyMatrixWithVec4d(result, T, v);
}

void updateVertices(const Triangle& triangle, double result[4], int i)
{
    vertices[triangle.vertexIds[i]].x = result[0];
    vertices[triangle.vertexIds[i]].y = result[1];
    vertices[triangle.vertexIds[i]].z = result[2];
}

////////////////////////VIEWING-TRANSFORMATIONS/////////////////////////////////

// Multiply Camera Transformation Matrix with Vertice
void cameraTransformation(double v[4])
{
    double temp[4];
    multiplyMatrixWithVec4d(temp, CAM, v);
    for(int i=0; i < 4 ; i++)
    	v[i] = temp[i];
    //memcpy(v, temp, sizeof(double)*4);
}

// Multiply Perspective Transformation Matrix with Vertice
void perspectiveTransformation(double v[4])
{
    double temp[4];
    multiplyMatrixWithVec4d(temp, PER, v);
        for(int i=0; i < 4 ; i++)
    	v[i] = temp[i];
    //memcpy(v, temp, sizeof(double)*4);
}

void viewPortTransformation(double v[4], const Camera& cam)
{
    double temp[4];
    multiplyMatrixWithVec4d(temp, VIP, v);
    for(int i=0; i < 4 ; i++)
    {
        v[i] = temp[i];
    }
    if(v[0] > cam.sizeX)
        std::cout << "ZRAONRA"<< std::endl;
    if(v[1] > cam.sizeY)
        std::cout << "ZRsdasdasdasdasAONRA"<< std::endl;
    //memcpy(v, temp, sizeof(double)*4);
}

////////////////////////VIEWING-TRANSFORMATIONS-END/////////////////////////////

void perspectiveDivide(double result[4])
{
    int i = 0;
    double w = result[3];
    if(w == 0)
        return;
    for(i=0; i < 3;i++)
        result[i] /= w;
    result[3] = 1;
}

//////////////////////////////RASTERIZATION////////////////////////////////////

// It is written for f01 but can be extended to any fXX 
double f(int x, int y, int x0, int x1, int y0, int y1)
{
    return x*(y0-y1) + y*(x1-x0) + x0*y1 - y0*x1;
}

void draw(int x, int y, double r, double b, double g)
{
    image[x][y].r = r;
    image[x][y].b = b; 
    image[x][y].g = g; 
}

// Vertices of Triangles in counter-clockwise order
// a->0, b->2, c->1
void triangleRast(const Triangle& tri, const Camera& cam)
{
    Vec3 X0 = vertices[tri.vertexIds[0]];
    int x0 = X0.x;
    int y0 = X0.y;
    Color c0 = colors[X0.colorId];
     
    Vec3 X1 = vertices[tri.vertexIds[1]];
    int x1 = X1.x;
    int y1 = X1.y;
    Color c1 = colors[X1.colorId];
    
    Vec3 X2 = vertices[tri.vertexIds[2]];
    int x2 = X2.x;
    int y2 = X2.y;
    Color c2 = colors[X2.colorId];
    
    double alpha, beta, gama;
    
    int xmin = max(min(x0, x1, x2),0);
    int xmax = min(max(x0, x1, x2), cam.sizeX-1);
    int ymin = max(min(y0, y1, y2),0);
    int ymax = min(max(y0, y1, y2), cam.sizeY-1);
    
    //std::cout << xmin << " " << xmax <<  " " << ymin <<  " " << ymax<< std::endl;

    for(int y=ymin; y < ymax; y++)
    {
        for(int x=xmin; x < xmax; x++)
        {
            //std::cout << "rasterization loop" << std::endl;
            double f00 = f(x0, y0, x1, x2, y1, y2);
            double f11 = f(x1, y1, x2, x0, y2, y0);
            double f22 = f(x2, y2, x0, x1, y0, y1);
            
            alpha = double(f(x, y, x1, x2, y1, y2)) / f00;
            beta  = double(f(x, y, x2, x0, y2, y0)) / f11;
            gama  = double(f(x, y, x0, x1, y0, y1)) / f22;
            
            if(alpha >= 0 && beta >= 0 && gama >= 0)
            {
                draw(x, y, 
                     alpha*c0.r + beta*c1.r + gama*c2.r,
                        alpha*c0.b + beta*c1.b + gama*c2.b,
                            alpha*c0.g + beta*c1.g + gama*c2.g);
           }
        }
    }
}

void swapPoints(int& x0, int& x1, int& y0, int& y1, Color& c0, Color& c1)
{
    int tempx = x0;
    x0 = x1;
    x1 = tempx;

    int tempy = y0;
    y0 = y1;
    y1 = tempy;
    
    Color temp = c0;
    c0 = c1;
    c1 = temp;
}

void swap(int& y0, int& y1)
{
    int temp = y1;
    y1 = y0;
    y0 = temp;
}

void midPointHelper(int x0, int y0, int x1, int y1, Color c0, Color c1)
{
    bool revert = (abs(y1-y0) > abs(x1-x0)); 

    // If slope is bigger than 1 or less than -1
    // We change the roles of x and y
    if(revert)
    {
        swap(x0,y0);
        swap(x1,y1);
    }
    // Always draw from x0 to x1
    // We also change the colors
    if(x0 > x1)
        swapPoints(x0,x1,y0,y1,c0,c1);
    
    double d = (x1-x0) / 2.0;
    double dy = abs(y1-y0);
    double dx = x1-x0;
     
    Color c = c0;
    Color dc;
    int x = x0;
    int y = y0;
    int incry = 1;

    // Remember we are drawing line from Point  0 to 1
    // If line is going in -y direction change the y increment
    if(y0 >= y1) 
        incry = -1;
    
    if(x1==x0)
    {
        std::cout << "Drawing line with infinite slope" << std::endl;
        return;
    }

    // Set color change between each pixel
    dc.r = (c1.r - c0.r)/(x1-x0);
    dc.b = (c1.b - c0.b)/(x1-x0);
    dc.g = (c1.g - c0.g)/(x1-x0);

    for(int x=x0; x <= x1; x++)
    {
        if(revert)
            draw(y, x, c.r, c.b, c.g);
        else
            draw(x, y, c.r, c.b, c.g);
        
        d -= dy;
        if(d < 0)
        {
            y += incry;
            d += dx;
        }
        c.r += dc.r;
        c.b += dc.b;
        c.g += dc.g;
    }
}


void midPoint(const Triangle& tri)
{
    Vec3 X0 = vertices[tri.vertexIds[0]];
    int x0 = X0.x;
    int y0 = X0.y;
    Color c0 = colors[X0.colorId];
     
    Vec3 X1 = vertices[tri.vertexIds[1]];
    int x1 = X1.x;
    int y1 = X1.y;
    Color c1 = colors[X1.colorId];
    
    Vec3 X2 = vertices[tri.vertexIds[2]];
    int x2 = X2.x;
    int y2 = X2.y;
    Color c2 = colors[X2.colorId];
 
    midPointHelper(x0,y0, x1,y1, c0, c1);
    midPointHelper(x1,y1, x2,y2, c1, c2);
    midPointHelper(x2,y2, x0,y0, c2, c0);
}

// Return 1 means that this object is not culled so it can be rasterized
int culling(const Vec3& normal, const Vec3& v0)
{
    Vec3 minusV0 = multiplyVec3WithScalar(v0,-1);
    if(dotProductVec3(minusV0, normal) >= 0)
        return 0;
    return 1;       
}


void findCenterOfTriangle(const Triangle& tri, Vec3& v)
{
    double determinant;

    Vec3 X0 = vertices[tri.vertexIds[0]];
    int x0 = X0.x;
    int y0 = X0.y;
     
    Vec3 X1 = vertices[tri.vertexIds[1]];
    int x1 = X1.x;
    int y1 = X1.y;
    
    Vec3 X2 = vertices[tri.vertexIds[2]];
    int x2 = X2.x;
    int y2 = X2.y;
 
    v.x = (x0+x1+x2)/3;
    v.y = (y0+y1+y2)/3;
    v.z = 1;
/*
    int x1_xo = x1-x0;
    int x2_xo = x2-x0;
    int y1_y0 = y1-y0;
    int y2_y0 = y2-y0;
    determinant =    
*/
}

//////////////////////////////RASTERIZATION////////////////////////////////////


// TODO Scaling is apllied after rasterization ????
// For each vertice of the Triangle we apply transformation
void transformationHelper(const Model& model, const Triangle& triangle, const Camera& cam)
{
    double result[4];
    Vec3 center;
    Vec3 normal;
    // For each Vertice of the triangle
    for(int k=0; k < 3; k++) 
    {
    	// If transformation is applied before continue

    	if(transformed[triangle.vertexIds[k]] == true)
    		continue;

        double v[4] = {vertices[triangle.vertexIds[k]].x,
                       vertices[triangle.vertexIds[k]].y,
                       vertices[triangle.vertexIds[k]].z,
                       1};

        for(int i=0; i < model.numberOfTransformations; i++)
        {
            char type = model.transformationTypes[i]; 
            int transformationID = model.transformationIDs[i];
            switch(type) {
                case('s'):
                    scale(scalings[transformationID], v, result);
                    break;
                case('r'):
                    rotate(rotations[transformationID], v, result);
                    break;
                case('t'):
                    translate(translations[transformationID], v, result);
                    break;
                default:
                    std::cout << "What kind of an transformation is that?" << std::endl;
            }
            
            for(int j=0; j < 4; j++)
        		v[j] = result[j];
        	
        }
        cameraTransformation(v);
        perspectiveTransformation(v);
        perspectiveDivide(v);
        viewPortTransformation(v,cam);
        updateVertices(triangle, v, k);
        transformed[triangle.vertexIds[k]] = true;
    }
    findCenterOfTriangle(triangle, center);
    findNormal(triangle, normal);
    if(model.type)
    {
        if(backfaceCullingSetting)
        {
            if(culling(normal, center))
                triangleRast(triangle, cam);
        }
        else
            triangleRast(triangle, cam);
    }
    else
    {

        if(backfaceCullingSetting)
        {
            if(culling(normal, center))
                midPoint(triangle);
        }
        else
            midPoint(triangle);
    }
}

// Modeling + Camera + Perspective + ViewPort Transformations are done 
void doTransformations(const Camera& cam)
{
    for(int i=0; i < numberOfModels; i++)
    {   
        Model model_i = models[i];
        for(int j=0; j < model_i.numberOfTriangles; j++)
        {
            Triangle& triangle_j = model_i.triangles[j];
            transformationHelper(model_i, triangle_j, cam);
        }
        // We reset the vertices to its original values for each model

        for(int i=0; i <= numberOfVertices; i++)
            vertices[i] = backUp[i];

        for(int j=0; j <= numberOfVertices; j++)
          	transformed[j] = false;
    }
}

void initializeCameraTransform(const Camera& cam)
{
    makeIdentityMatrix(CAM);
    CAM[0][0] = cam.u.x;
    CAM[0][1] = cam.u.y;
    CAM[0][2] = cam.u.z;
    
    CAM[1][0] = cam.v.x;
    CAM[1][1] = cam.v.y;
    CAM[1][2] = cam.v.z;
    
    CAM[2][0] = cam.w.x;
    CAM[2][1] = cam.w.y;
    CAM[2][2] = cam.w.z;
    
    CAM[0][3] = -(cam.u.x*cam.pos.x + cam.u.y*cam.pos.y + cam.u.z*cam.pos.z);
    CAM[1][3] = -(cam.v.x*cam.pos.x + cam.v.y*cam.pos.y + cam.v.z*cam.pos.z);
    CAM[2][3] = -(cam.w.x*cam.pos.x + cam.w.y*cam.pos.y + cam.w.z*cam.pos.z);
}

void initializePerspectiveTransformation(const Camera& cam)
{
    // Perspective to Ortographic Matrix
    double Mp2o[4][4];
    makeIdentityMatrix(Mp2o);
    // Ortographic Transformation: Transform given viewing Volume to CVV 
    double Morth[4][4];
    makeIdentityMatrix(Morth);

    Mp2o[0][0] = cam.n;
    Mp2o[1][1] = cam.n;
    Mp2o[2][2] = cam.f + cam.n;
    Mp2o[2][3] = cam.f * cam.n;
    Mp2o[3][2] = -1;
    Mp2o[3][3] = 0;
    
    Morth[0][0] = 2.0/(cam.r - cam.l);
    Morth[0][3] = - (cam.r + cam.l) / (cam.r - cam.l);
    Morth[1][1] = 2.0/(cam.t - cam.b);
    Morth[1][3] = - (cam.t + cam.b) / (cam.t - cam.b);
    Morth[2][2] = -2.0/(cam.f - cam.n);
    Morth[2][3] = - (cam.f + cam.n) / (cam.f - cam.n);
    
    multiplyMatrixWithMatrix(PER, Morth, Mp2o);
}

void initializeViewPort(const Camera& cam)
{
    makeIdentityMatrix(VIP);
    VIP[0][0] = cam.sizeX/2.0;
    VIP[0][3] = (cam.sizeX-1)/2.0;
    VIP[1][1] = cam.sizeY/2.0;
    VIP[1][3] = (cam.sizeY-1)/2.0;
    VIP[2][2] = 0.5;
    VIP[2][3] = 0.5;
    VIP[3][3] = 1;
}

void forwardRenderingPipeline(const Camera& cam) {

	for(int i=0; i <= numberOfVertices; i++)
		transformed[i] = false;

    initializeCameraTransform(cam);
    initializePerspectiveTransformation(cam);
    initializeViewPort(cam);
    doTransformations(cam);

    for(int i=0; i <= numberOfVertices; i++)
    	vertices[i] = backUp[i];
}

int main(int argc, char **argv) {
    int i, j;

    if (argc < 2) {
        std::cout << "Usage: ./rasterizer <scene file> <camera file>" << std::endl;
        return 1;
    }

    // read camera and scene files
    readSceneFile(argv[1]);

    readCameraFile(argv[2]);

    for(int i=0; i <= numberOfVertices; i++)
    	backUp[i] = vertices[i];


    image = 0;

    for (i = 0; i < numberOfCameras; i++) {

        // allocate memory for image
        if (image) {
			for (j = 0; j < cameras[i].sizeX; j++) {
		        delete image[j];
		    }

			delete[] image;
		}

        image = new Color*[cameras[i].sizeX];

        if (image == NULL) {
            std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
            exit(1);
        }

        for (j = 0; j < cameras[i].sizeX; j++) {
            image[j] = new Color[cameras[i].sizeY];
            if (image[j] == NULL) {
                std::cout << "ERROR: Cannot allocate memory for image." << std::endl;
                exit(1);
            }
        }


        // initialize image with basic values
        initializeImage(cameras[i]);

        // do forward rendering pipeline operations
        forwardRenderingPipeline(cameras[i]);

        // generate PPM file
        writeImageToPPMFile(cameras[i]);

        // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
        // Notice that os_type is not given as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
        // Change os_type to 1 or 2, after being sure that you have ImageMagick installed.
        convertPPMToPNG(cameras[i].outputFileName, 99);
    }

    return 0;

}
