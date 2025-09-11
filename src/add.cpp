#include <iostream>
#include <fstream>
#include <sstream>

#include <CL/cl.h>
 
const int ARRAY_SIZE = 1000;
 
/**
 * @brief 1. 选择OpenCL平台并创建一个上下文
 * @note 平台（Platform）是指主机和OpenCL管理框架下的若干个设备构成的可以运行OpenCL程序的完整硬件系统
 *       这个是跑OpenCL程序的基础，所以第一步要选择一个可用的OpenCL品台。一台机器上可以有不止一个这样的品台，一个平台也可以有不止一个GPU。
 */
cl_context CreateContext()
{
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;
	cl_context context = NULL;
 
	//选择可用的平台中的第一个
	errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0)
	{
		std::cerr << "Failed to find any OpenCL platforms." << std::endl;
		return NULL;
	}
 
	//创建一个OpenCL上下文环境
	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
		0
	};
	context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
 
	return context;
}
 
 
/**
 * @brief 2. 选择设备并创建命令队列
 * 
 * @note 选择平台并创建好OpenCL上下文环境之后，要做的事选择运行时用到的设备
 *       还要创建一个命令队列，命令队列里定义了设备要完成的操作，以及各个操作的运行次序。
 */
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
{
	cl_int errNum;
	cl_device_id *devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;
 
	// 获取设备缓冲区大小
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
 
	if (deviceBufferSize <= 0)
	{
		std::cerr << "No devices available.";
		return NULL;
	}
 
	// 为设备分配缓存空间
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
 
	//选取可用设备中的第一个
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
 
	*device = devices[0];
	delete[] devices;
	return commandQueue;
}
 
 
/**
 * @brief 3. 创建和构建程序对象
 * 
 * @note 程序对象用来存储与上下文相关联的设备的已编译可执行代码，同时也完成内核源代码的加载编译工作。
 */
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
	cl_int errNum;
	cl_program program;
 
	std::ifstream kernelFile(fileName, std::ios::in);
	if (!kernelFile.is_open())
	{
		std::cerr << "Failed to open file for reading: " << fileName << std::endl;
		return NULL;
	}
 
	std::ostringstream oss;
	oss << kernelFile.rdbuf();
 
	std::string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();

  // 创建一个函数对象，并且将"cl"对应的内核代码加载到该对象中
	program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, NULL, NULL);
 
  // 编译指定程序对象中的内核源代码，并将编译后的可执行代码加载到与程序对象相关联的设备中
	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
 
	return program;
}
 
/**
 * @brief 4. 创建内核和内存对象
 * 
 * @note 要执行程序对象中的已编译成功的内核运算，需要在内存中创建内核并分配内核函数的参数，在GPU上定义内存对象并分配存储空间。
 */
bool CreateMemObjects(cl_context context, cl_mem memObjects[3],
	float *a, float *b)
{
	memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * ARRAY_SIZE, a, NULL);
	memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * ARRAY_SIZE, b, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
		sizeof(float) * ARRAY_SIZE, NULL, NULL);
	return true;
}
 
 
/**
 * @brief 6. 读取执行结果并释放OpenCL资源
 * 
 * @note 内核执行完成之后，需要把数据从GPU拷贝到CPU中，供主机进一步处理，所有者写工作完成之后需要释放所有的OpenCL资源。
 */
void Cleanup(cl_context context, cl_command_queue commandQueue,
	cl_program program, cl_kernel kernel, cl_mem memObjects[3])
{
	for (int i = 0; i < 3; i++)
	{
		if (memObjects[i] != 0)
			clReleaseMemObject(memObjects[i]);
	}
	if (commandQueue != 0)
		clReleaseCommandQueue(commandQueue);
 
	if (kernel != 0)
		clReleaseKernel(kernel);
 
	if (program != 0)
		clReleaseProgram(program);
 
	if (context != 0)
		clReleaseContext(context);
}
 
int main(int argc, char** argv)
{
	cl_context context = 0;
	cl_command_queue commandQueue = 0;
	cl_program program = 0;
	cl_device_id device = 0;
	cl_kernel kernel = 0;
	cl_mem memObjects[3] = { 0, 0, 0 };
	cl_int errNum;
 
	// 一、选择OpenCL平台并创建一个上下文
	context = CreateContext();
 
	// 二、 创建设备并创建命令队列
	commandQueue = CreateCommandQueue(context, &device);
 
	//创建和构建程序对象
	program = CreateProgram(context, device, "/mnt/workspace/cgz_workspace/Exercise/opencl_example/src/add.cl");
 
	// 四、 创建OpenCL内核并分配内存空间
	kernel = clCreateKernel(program, "hello_kernel", NULL);
 
	//创建要处理的数据
	float result[ARRAY_SIZE];
	float a[ARRAY_SIZE];
	float b[ARRAY_SIZE];
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		a[i] = (float)i;
		b[i] = (float)(ARRAY_SIZE - i);
	}
 
	//创建内存对象
	if (!CreateMemObjects(context, memObjects, a, b))
	{
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}
 
	// 五、 设置内核数据并执行内核
	errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
	errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
	errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);
 
	size_t globalWorkSize[1] = { ARRAY_SIZE };
	size_t localWorkSize[1] = { 1 };
 
	errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL,
		globalWorkSize, localWorkSize,
		0, NULL, NULL);
 
	// 六、 读取执行结果并释放OpenCL资源
	errNum = clEnqueueReadBuffer(commandQueue, memObjects[2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result,
		0, NULL, NULL);
 
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		std::cout << result[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "Executed program succesfully." << std::endl;
	getchar();
	Cleanup(context, commandQueue, program, kernel, memObjects);
 
	return 0;
}