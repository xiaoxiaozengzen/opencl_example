#include <iostream>
#include <string>
#include <vector>

#include <CL/opencl.hpp>

static cl_int PrintPlatformInfoSummary(cl::Platform platform) {
  std::cout << "\tName:           " << platform.getInfo<CL_PLATFORM_NAME>()
            << "\n";
  std::cout << "\tVendor:         " << platform.getInfo<CL_PLATFORM_VENDOR>()
            << "\n";
  std::cout << "\tDriver Version: " << platform.getInfo<CL_PLATFORM_VERSION>()
            << "\n";
  std::cout << "\tProfile:        " << platform.getInfo<CL_PLATFORM_PROFILE>()
            << "\n";
  std::cout << "\tExtensions:     " << platform.getInfo<CL_PLATFORM_EXTENSIONS>()
            << "\n";

  return CL_SUCCESS;
}

static void PrintDeviceType(const std::string &label, cl_device_type type) {
  std::cout << label << ((type & CL_DEVICE_TYPE_DEFAULT) ? "DEFAULT " : "")
            << ((type & CL_DEVICE_TYPE_CPU) ? "CPU " : "")
            << ((type & CL_DEVICE_TYPE_GPU) ? "GPU " : "")
            << ((type & CL_DEVICE_TYPE_ACCELERATOR) ? "ACCELERATOR " : "")
            << ((type & CL_DEVICE_TYPE_CUSTOM) ? "CUSTOM " : "") << "\n";
}

static cl_int PrintDeviceInfoSummary(const std::vector<cl::Device> devices) {
  for (size_t i = 0; i < devices.size(); i++) {
    std::cout << "Device[" << i << "]:\n";

    cl_device_type deviceType = devices[i].getInfo<CL_DEVICE_TYPE>();
    PrintDeviceType("\tType:           ", deviceType);

    std::cout << "\tName:           " << devices[i].getInfo<CL_DEVICE_NAME>()
              << "\n";
    std::cout << "\tVendor:         " << devices[i].getInfo<CL_DEVICE_VENDOR>()
              << "\n";
    std::cout << "\tDevice Version: " << devices[i].getInfo<CL_DEVICE_VERSION>()
              << "\n";
    std::cout << "\tDevice Profile: " << devices[i].getInfo<CL_DEVICE_PROFILE>()
              << "\n";
    std::cout << "\tDriver Version: " << devices[i].getInfo<CL_DRIVER_VERSION>()
              << "\n";
    cl::Platform platform = devices[i].getInfo<CL_DEVICE_PLATFORM>();
    std::cout << "\tVender ID:      " << devices[i].getInfo<CL_DEVICE_VENDOR_ID>()
              << "\n";
    std::cout << "\tMax Compute Units: " << devices[i].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() 
              << "\n";
  }

  return CL_SUCCESS;
}

int main(int, char **) {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  std::cout << "Enumerated " << platforms.size() << " platforms.\n\n";

  for (size_t i = 0; i < platforms.size(); i++) {
    std::cout << "Platform[" << i << "]:\n";
    PrintPlatformInfoSummary(platforms[i]);

    std::vector<cl::Device> devices;
    platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

    PrintDeviceInfoSummary(devices);
    std::cout << "\n";
  }

  std::cout << "Done.\n";

  return 0;
}
