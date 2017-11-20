#include "arguments.hpp"
#include "clerr.hpp"
#include "clhelpers.hpp"
#include "gpu.hpp"
#include "pnm.hpp"

#include <CL/cl.hpp>
#include <netpbm/pam.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

cl::Context context;
cl::CommandQueue queue;
cl::Kernel blur;

bool gpu_init(void)
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.size() == 0) {
        std::cerr << "Error: No OpenCL platforms found." << std::endl;
        return false;
    } else if (verbose) {
        std::cerr << "Found " << platforms.size() << " OpenCL platform(s)" << std::endl;
    }

    cl_uint max_compute_units = 0;
    cl::Platform best_platform;
    cl::Device best_device;

    unsigned number_of_devices = 0;

    for (unsigned i=0; i<platforms.size(); ++i) {
        if (verbose) {
            std::cerr << "Searching for devices on platform " << platforms[i].getInfo<CL_PLATFORM_NAME>() << ':' << std::endl;
        }

        std::vector<cl::Device> devices;
        platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

        if (devices.size() == 0) {
            if (verbose) {
                std::cerr << "- No devices found." << std::endl;
            }
            continue;
        }

        number_of_devices += devices.size();

        cl_uint max_platform_compute_units = 0;
        cl_uint device_index = 0;

        for (unsigned j=0; j<devices.size(); ++j) {
            if (verbose) {
                std::cerr << "-- " << devices[j].getInfo<CL_DEVICE_NAME>() << std::endl;
            }

            cl_uint ncus = devices[j].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();

            if (ncus >= max_platform_compute_units) {
                max_platform_compute_units = ncus;
                device_index = j;
            }
        }

        if (max_platform_compute_units >= max_compute_units) {
            max_compute_units = max_platform_compute_units;
            best_platform = platforms[i];
            best_device = devices[device_index];
        }
    }

    if (number_of_devices == 0) {
        std::cerr << "No devices found on any available platform" << std::endl;
        return false;
    }

    if (verbose) {
        std::cerr << "Using device " << best_device.getInfo<CL_DEVICE_NAME>();
        std::cerr << " on platform " << best_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }

    context = cl::Context({best_device});

    queue = cl::CommandQueue(context, best_device);

    return build_kernels(best_device);
}

cl::Image2D create_output_image(void)
{
    const cl_mem_flags flags = CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY;

    cl::ImageFormat format(CL_RGBA, CL_FLOAT);

    return cl::Image2D(context, flags, format, image_width, image_height, 0, nullptr, nullptr);
}

void set_kernel_args(cl::Kernel &kernel, const cl::Image2D &input, const cl::Image2D &output, float param)
{
    if (verbose) {
        std::cerr << "Setting kernel arguments" << std::endl;
    }

    HANDLE_CLERR(kernel.setArg(0, input));

    HANDLE_CLERR(kernel.setArg(1, output));

    HANDLE_CLERR(kernel.setArg(2, param));
 }

void execute_kernel(const cl::Kernel &kernel)
{
    if (verbose) {
        std::cerr << "Executing kernel " << kernel.getInfo<CL_KERNEL_FUNCTION_NAME>() << std::endl;
    }

    const cl::NDRange globalWorkSize(image_width, image_height);

    auto t0 = std::chrono::high_resolution_clock::now();

    HANDLE_CLERR(queue.enqueueNDRangeKernel(kernel, 0, globalWorkSize));

    queue.finish();

    auto t1 = std::chrono::high_resolution_clock::now();

    const double t = std::chrono::duration<double>(t1 - t0).count();

    if (verbose) {
        std::cerr << "-- Kernel took " << t << " seconds" << std::endl;
    }
}
