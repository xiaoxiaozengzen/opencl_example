#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>

#include <CL/cl.h>

/**
 * ========== 图像旋转原理 ==========
 * 图像旋转定义为：将图像绕某个点旋转一定的角度。通常是指绕图像的中心点以逆时针方向旋转。
 * 假设图像的左上角为(l, t)，右下角为(r, b)，中心点为(cx, cy)，旋转角度为angle。
 * 对于图像中的每个像素点(x, y)，其旋转后的新坐标(x',
 * y')可以通过以下公式计算得到： x' = (x - cx) * cos(angle) - (y - cy) *
 * sin(angle) + cx y' = (x - cx) * sin(angle) + (y - cy) * cos(angle) + cy
 */

/**
 * @brief 图像旋转函数
 * @note OpenCL C Kernel 代码见 rotate.cl
 */
void rotate(unsigned char *inbuf, unsigned char *outbuf, int w, int h,
            float sinTheta, float cosTheta) {
  int i, j;
  int xc = w / 2;
  int yc = h / 2;
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      int xpos = (j - xc) * cosTheta - (i - yc) * sinTheta + xc;
      int ypos = (j - xc) * sinTheta + (i - yc) * cosTheta + yc;
      if (xpos >= 0 && ypos >= 0 && xpos < w && ypos < h)
        outbuf[ypos * w + xpos] = inbuf[i * w + j];
    }
  }
}

/**
 * 使用OpenCL进行编程的一般流程：
 *  - Platform
 *    - 1. 查询并选择一个Platform
 *    - 2. 在Platform上创建一个Context
 *    - 3. 在Context上查询并创建一个或多个device
 *  - Running Time
 *    - 4. 加载OpenCL内核程序并创建一个Program对象
 *    - 5. 为指定的device编译Program中的kernel
 *    - 6. 创建指定名字的kernel对象
 *    - 7. 为kernel创建内存对象
 *    - 8. 设置kernel参数
 *    - 9. 在指定的device上创建一个Command Queue
 *    - 10. 将要执行的kernel加入Command Queue
 *    - 11. 读取kernel执行结果，返回给host
 *    - Cleanup
 *
 */

static const int WIDTH = 6;
static const int HEIGHT = 6;
static const int IMAGE_SIZE = WIDTH * HEIGHT;
static const float SIN = 1.0;
static const float COS = 0.0;

int main(int argc, char **argv) {
  /*********************************** 查询并选择一个Platform ************************************/
  // 1.1 获取系统中所有的Platform
  cl_int status = 0;
  cl_uint numPlatforms;
  cl_platform_id platform = NULL;
  status = clGetPlatformIDs(0, NULL, &numPlatforms);
  if (status != CL_SUCCESS) {
    std::cout << "clGetPlatformIDs failed (1)" << std::endl;
    return 1;
  }

  // 1.2 根据查询到的Platform数量，申请相应大小的内存，并获取所有Platform ID
  if (numPlatforms > 0) {
    cl_platform_id *platforms =
        (cl_platform_id *)malloc(numPlatforms * sizeof(cl_platform_id));
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    if (status != CL_SUCCESS) {
      printf("Error: Getting Platform Ids.(clGetPlatformIDs)\n");
      return -1;
    }
    std::cout << "Number of platforms: " << numPlatforms << std::endl;

    for (unsigned int i = 0; i < numPlatforms; ++i) {
      char pbuff[100];
      status = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR,
                                 sizeof(pbuff), pbuff, NULL);
      std::cout << "Platform " << i << " : Vendor" << pbuff << std::endl;
      platform = platforms[i];
    }

    delete platforms;
  }

  /*********************************** 在Platform上创建一个Context ************************************/
  // 2.1. 通过platform得到相应的context properties
  cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
                                  (cl_context_properties)platform, 0};
  cl_context_properties *cprops = (NULL == platform) ? NULL : cps;
  
  // 2.2. 创建context
  cl_context context = clCreateContextFromType(cprops, CL_DEVICE_TYPE_GPU, NULL,
                                               NULL, &status);
  if (status != CL_SUCCESS) {
    std::cout << "clCreateContextFromType failed." << std::endl;
    return 1;
  }

  /*********************************** 在Context上查询并创建一个或多个device ************************************/
  // 3.1. 查询可用的device
  size_t deviceListSize;
  status = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL,
                            &deviceListSize);
  if (status != CL_SUCCESS) {
    std::cout << "clGetContextInfo failed (1)." << std::endl;
    return 1;
  }
  std::cout << "deviceListSize = " << deviceListSize << std::endl;
  cl_device_id *devices =
      (cl_device_id *)malloc(deviceListSize * sizeof(cl_device_id));
  if(devices == NULL) {
    std::cout << "Failed to allocate memory for devices." << std::endl;
    return 1;
  }
  status = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceListSize,
                            devices, NULL);
  if (status != CL_SUCCESS) {
    std::cout << "clGetContextInfo failed (2)." << std::endl;
    return 1;
  }

  /************************************* Running Time **********************************/
  // 4.1. 加载OpenCL内核程序并创建一个Program对象
  std::ifstream kernelFile("/mnt/workspace/cgz_workspace/Exercise/opencl_example/src/rotate.cl", std::ios::in);
  if(!kernelFile.is_open()) {
    std::cout << "Failed to open kernel file." << std::endl;
    return 1;
  }
  std::stringstream ss;
  ss << kernelFile.rdbuf();
  std::string kernelSource = ss.str();
  const char *kernelSourceCStr = kernelSource.c_str();
  size_t kernelSourceSize = kernelSource.size();
  kernelFile.close();
  cl_program program = clCreateProgramWithSource(context, 1,
                                                 (const char **)&kernelSourceCStr,
                                                 NULL, NULL);
  if (status != CL_SUCCESS) {
    std::cout << "clCreateProgramWithSource failed." << std::endl;
    return 1;
  }
  // 4.2. 为指定的device编译Program中的kernel
  status = clBuildProgram(program, 1, &devices[0], NULL, NULL, NULL);
  if (status != CL_SUCCESS) {
    std::cout << "clBuildProgram failed." << std::endl;
    return 1;
  }
  // 4.3. 创建指定名字的kernel对象
  cl_kernel kernel = clCreateKernel(program, "image_rotate", &status);
  if (status != CL_SUCCESS) {
    std::cout << "clCreateKernel failed." << std::endl;
    return 1;
  }
  // 4.4. 为kernel创建内存对象
  int * outbuffer = (int *)malloc(WIDTH * HEIGHT * sizeof(int));
  memset(outbuffer, 0, WIDTH * WIDTH * sizeof(int));
  int * inbuffer = new int[IMAGE_SIZE];
  for (int i = 0; i < IMAGE_SIZE; i++) {
    inbuffer[i] = i;
  }
  cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                      IMAGE_SIZE * sizeof(int), inbuffer, &status);
  if (status != CL_SUCCESS) {
    std::cout << "clCreateBuffer failed." << std::endl;
    return 1;
  }                              
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                       IMAGE_SIZE * sizeof(int),
                                       NULL, &status);
  if (status != CL_SUCCESS) {
    std::cout << "clCreateBuffer failed." << std::endl;
    return 1;
  }                                       
  cl_int widthParam = WIDTH;
  cl_int heightParam = HEIGHT;
  cl_float sinParam = SIN;
  cl_float cosParam = COS;
  // 4.5. 设置kernel参数
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
  if (status != CL_SUCCESS) {
    std::cout << "clSetKernelArg failed." << __LINE__ << std::endl;  
    return 1;
  }  
  status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
  if (status != CL_SUCCESS) {
    std::cout << "clSetKernelArg failed." << __LINE__ << std::endl;  
    return 1;
  }  
  status = clSetKernelArg(kernel, 2, sizeof(cl_int), (void*)&widthParam);
  if (status != CL_SUCCESS) {
    std::cout << "clSetKernelArg failed." << __LINE__ << std::endl;  
    return 1;
  }  
  status = clSetKernelArg(kernel, 3, sizeof(cl_int), &heightParam);
  if (status != CL_SUCCESS) {
    std::cout << "clSetKernelArg failed." << __LINE__ << std::endl;  
    return 1;
  }    
  status = clSetKernelArg(kernel, 4, sizeof(cl_float), &sinParam);
  if (status != CL_SUCCESS) {
    std::cout << "clSetKernelArg failed." << __LINE__ << std::endl;  
    return 1;
  }    
  status = clSetKernelArg(kernel, 5, sizeof(cl_float), &cosParam);
  if (status != CL_SUCCESS) {
    std::cout << "clSetKernelArg failed." << __LINE__ << std::endl;  
    return 1;
  }  
  // 4.6. 在指定的device上创建一个Command Queue
  cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
  if (status != CL_SUCCESS) {
    std::cout << "clCreateCommandQueue failed." << std::endl;
    return 1;
  }
  // 4.7. 将要执行的kernel加入Command Queue
  size_t globalThreads[2] = {WIDTH, HEIGHT};
  size_t localThreads = {1};
  /**
   * @param command_queue 命令队列
   * @param kernel 要执行的 kernel
   * @param work_dim 工作维度（如 1、2、3）
   * @param global_work_offset 全局偏移（通常为 NULL）
   * @param global_work_size 全局工作项数量（每个维度的线程总数）
   * @param local_work_size 局部工作项数量（每个工作组的线程数）
   * @param num_events_in_wait_list 等待的事件数量
   * @param event_wait_list 等待的事件列表
   * @param event 返回的事件句柄
   */
  status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL,
                                  globalThreads, &localThreads, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    std::cout << "clEnqueueNDRangeKernel failed." << std::endl;
    return 1;
  }
  // 4.7.1. 确认command queue中的命令已经执行完毕
  status = clFinish(commandQueue);
  if (status != CL_SUCCESS) {
    std::cout << "clFinish failed." << std::endl;
    return 1;
  }
  // 4.8. 读取kernel执行结果，返回给host
  status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0,
                               WIDTH * HEIGHT * sizeof(int), outbuffer, 0, NULL, NULL);
  if (status != CL_SUCCESS) {
    std::cout << "clEnqueueReadBuffer failed." << std::endl;
    return 1;
  }
  for(int i = 0; i < HEIGHT; i++) {
    for(int j = 0; j < WIDTH; j++) {
      std::cout << outbuffer[i * WIDTH + j] << " ";
    }
    std::cout << std::endl;
  }
  // 4.9. Cleanup
  clReleaseCommandQueue(commandQueue);
  clReleaseMemObject(outputBuffer);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseContext(context);
  free(devices);
  delete outbuffer;
  delete inbuffer;

  return 0;
}