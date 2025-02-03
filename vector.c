#include "/usr/include/CL/cl.h"
#include <stdio.h>
#include <stdlib.h>

const char *vectorSource = 
"__kernel void add(__global const float *v1, __global const float *v2, __global float *res)   {\n"
"int id = get_global_id(0);\n"
"res[id] = v1[id]+v2[id];\n"
"}\n"
;
int main() {
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_mem buffer1 = NULL, buffer2 = NULL, bufferres = NULL;
    cl_int ret;
    ret = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, NULL);

    command_queue = clCreateCommandQueue(context, device_id, 0, NULL);

    program = clCreateProgramWithSource(context, 1, (const char **)&vectorSource, NULL, NULL);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "add", NULL);
    const int N = 5;
    float arr1[N], arr2[N], result[N];
    for (int i = 0; i < N; i++) {
        arr1[i] = 2.0;
        arr2[i] = 3.0;
    }

    buffer1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(float), arr1, NULL);
    buffer2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(float), arr2, NULL);
    bufferres = clCreateBuffer(context, CL_MEM_WRITE_ONLY , N * sizeof(float), NULL, NULL);



    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer1);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer2);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferres);

    size_t global_item_size = N;

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(command_queue, bufferres, CL_TRUE, 0, N * sizeof(float), result, 0, NULL, NULL);
    clFinish(command_queue);
    printf("Result:\n");
    for (int i = 0; i < N; i++) {
        printf("%f ", result[i]);
    }
    printf("\n");

    return 0;
}
