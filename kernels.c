/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
/* 
 * Please fill in the following student struct 
 */
team_t team = {
    "e2099018",              /* Student ID */

    "EGEMEN BERK GALATALI",     /* full name */
    "e20999018@ceng.metu.edu.tr",  /* email address */

    "",                   /* leave blank */
    ""                    /* leave blank */
};


/***************
 * CONVOLUTION KERNEL
 ***************/

/******************************************************
 * Your different versions of the convolution functions  go here
 ******************************************************/

/* 
 * naive_conv - The naive baseline version of convolution 
 */
char naive_conv_descr[] = "naive_conv: Naive baseline implementation";
void naive_conv(int dim,int *src, int *ker,int *dst) {
    int i,j,k,l;

    for(i = 0; i < dim-8+1; i++)
        for(j = 0; j < dim-8+1; j++) {
            dst[j*dim+i] = 0;
            for(k = 0; k < 8; k++)
                for(l = 0; l < 8; l++) {
                    dst[j*dim+i] = dst[j*dim+i] +src[(j+l)*dim+(i+k)]*ker[l*dim+k];
                }
        }
            
        
}

/* 
 * convolution - Your current working version of convolution
 * IMPORTANT: This is the version you will be graded on
 */
char convolution_descr[] = "Dot product: Current working version";
void convolution(int dim,int *src, int *ker,int *dst) 
{
    //       naive_conv(dim,src,ker,dst);
    int i,j,k,sum,f,y,z,z1;//l
    z1 = dim - 7;
    for(i = 0; i < z1; i++){
        f = i*dim;
        for(j = 0; j < z1; j+=2) { 
            sum = 0;
                y = 0;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];

                y += dim;;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];

////////////

                y += dim;;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];

                y += dim;;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j]*ker[y];
                sum += src[z+j + 1]*ker[y + 1];
                sum += src[z+j + 2]*ker[y + 2];
                sum += src[z+j + 3]*ker[y + 3];
                sum += src[z+j + 4]*ker[y + 4];
                sum += src[z+j + 5]*ker[y + 5];
                sum += src[z+j + 6]*ker[y + 6];
                sum += src[z+j + 7]*ker[y + 7];


            
            dst [ f + j ] = sum;
        
     ////             j += 2;
     
      
            sum = 0;
                y = 0;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];

                y += dim;;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];

////////////

                y += dim;;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];

                y += dim;;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];
                y += dim;
                z = f+y;
                sum += src[z+j+1]*ker[y];
                sum += src[z+j+1 + 1]*ker[y + 1];
                sum += src[z+j+1 + 2]*ker[y + 2];
                sum += src[z+j+1 + 3]*ker[y + 3];
                sum += src[z+j+1 + 4]*ker[y + 4];
                sum += src[z+j+1 + 5]*ker[y + 5];
                sum += src[z+j+1 + 6]*ker[y + 6];
                sum += src[z+j+1 + 7]*ker[y + 7];


            
            dst [ f + j+1 ] = sum;
           
 /////////////////////////// j += 4       
        
        
        
        
        }
    }
}

/*********************************************************************
 * register_conv_functions - Register all of your different versions
 *     of the convolution functions  with the driver by calling the
 *     add_conv_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_conv_functions() {
    add_conv_function(&naive_conv, naive_conv_descr);   
    add_conv_function(&convolution, convolution_descr);   
    /* ... Register additional test functions here */
}




/***************
 * MATRIX MULTIP KERNEL
 ***************/

/******************************************************
 * Your different versions of the matrix multiplications  go here
 ******************************************************/

/* 
 * naive_matrix_multiplication - The naive baseline version of matrix multiplication 
 */
char naive_matrix_multiplication_descr[] = "Naive_matrix_multiplication: Naive baseline implementation";
void naive_matrix_multiplication(int dim,int *src, int *src2,int *dst) {
    int i,j,k;

    for(i = 0; i < dim; i++)
        for(j = 0; j < dim; j++) {
            dst[j*dim+i]=0;
            for(k = 0; k < dim; k++) 
                dst[j*dim+i] = dst[j*dim+i] + src[j*dim+k]*src2[i+k*dim];
        }    
}


/* 
 * matrix_multiplication - Your current working version of matrix_multiplication
 * IMPORTANT: This is the version you will be graded on
 */
char matrix_multiplication_descr[] = "Matrix multiplications: Current working version";
void matrix_multiplication(int dim,int *src, int *src2,int *dst) 
{
    int i,j,z,y,sum;//,k;
            //naive_matrix_multiplication(dim,src,src2,dst);

    //int block_size=32;
    int *temp = malloc(sizeof(int) * dim * dim );
    //if( dim == 32 || dim == 128)// || dim == 256)
    //    block_size = 8;
/*
    for(i=0; i < dim; i+=block_size){
        for(j=0; j < dim; j+=block_size){
            for(int k=i; k<i+block_size; k++){
                z = k*dim;
                for(int l=j; l < j+block_size; l++){
                    temp[k+l*dim] = src2[l + z];
                }
            }

        }
    }
*/  // TAKE TRANSPOSE OF SRC2
        for(i=0; i < dim;){
        z = i * dim;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;

        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        z += dim;
        i++;
        for(j=0; j < dim; j+=8){
            y = j*dim;
            temp[z + j]   = src2[y + i];
            temp[z + j+1] = src2[y+dim + i];
            temp[z + j+2] = src2[y+dim+dim + i];
            temp[z + j+3] = src2[y+dim+dim+dim + i];
            temp[z + j+4] = src2[y+dim+dim+dim +dim+ i];
            temp[z + j+5] = src2[y+dim+dim+dim +dim+dim+ i];
            temp[z + j+6] = src2[y+dim+dim+dim +dim+dim+dim+ i];
            temp[z + j+7] = src2[y+dim+dim+dim +dim+dim+dim+dim+ i];
            }
        i++;
 

    }

    //       MULTIPLY IN ROW-ORDER FOR BOTH SRC AND SRC2

    int *temc = dst;

    for(i=0; i < dim; i+=2){
        z = i*dim;
        for(j=0; j < dim; j+=4){
            y = j*dim;
            int *tema = src  + z;
            sum = 0;
            int *temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
            y += dim;
            tema = src  + z;
            sum = 0;
            temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
 
            y += dim;
            tema = src  + z;
            sum = 0;
            temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
            y += dim;
            tema = src  + z;
            sum = 0;
            temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
 
 
        }
////////////////////////////////
        z += dim;
        for(j=0; j < dim; j+=4){
            y = j*dim;
            int *tema = src  + z;
            sum = 0;
            int *temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
            y += dim;
            tema = src  + z;
            sum = 0;
            temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
 
            y += dim;
            tema = src  + z;
            sum = 0;
            temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
                sum +=  *tema * *temb;
                tema++;
                temb++;
                sum +=  *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
            y += dim;
            tema = src  + z;
            sum = 0;
            temb = temp + y;
            for(int k=0; k< dim; k+=8)
            {
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
                sum += *tema * *temb;
                tema++;
                temb++;
                sum += *tema * *temb;
                tema++;
                temb++; 
            }
            *temc++ = sum;
 
    }
    }
free(temp);
}

/*********************************************************************
 * register_matrix_multiplication_functions - Register all of your different versions
 *     of the matrix multiplication  with the driver by calling the
 *     add_matrix_multiplication_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_matrix_multiplication_functions() {
    add_matrix_multiplication_function(&naive_matrix_multiplication, naive_matrix_multiplication_descr);   
    add_matrix_multiplication_function(&matrix_multiplication, matrix_multiplication_descr);   
    /* ... Register additional test functions here */
}

