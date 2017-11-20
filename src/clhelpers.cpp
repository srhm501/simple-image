#include "arguments.hpp"
#include "clerr.hpp"
#include "clhelpers.hpp"
#include "gpu.hpp"
#include "resources.hpp"

#include <CL/cl.hpp>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>


namespace {
    /*
    std::string get_source(const std::string &filename)
    {
        if (verbose) {
            std::cerr << "Reading source from " << filename << std::endl;
        }

        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "File " << filename << " not found!" << std::endl;
        }

        return std::string(std::istreambuf_iterator<char>(file),
                           (std::istreambuf_iterator<char>()));
    }
    */

    void print_build_log(const cl::Program &p, const cl::Device &d)
    {
        cl_int error;
        std::cerr << p.getBuildInfo<CL_PROGRAM_BUILD_LOG>(d, &error) << std::endl;
        HANDLE_CLERR(error);
    }

    bool compile_kernel(cl::Kernel &kernel,
                        const cl::Device &dev,
                        const std::string &kernel_name,
                        const std::string &source_code)
    {
        const cl::Program::Sources source(1, std::make_pair(source_code.c_str(), source_code.length()+1));

        cl::Program program(context, source);

        program.build({dev}, "-cl-fast-relaxed-math");

        cl_int error;
        auto status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(dev, &error);
        HANDLE_CLERR(error);

        if (status != CL_BUILD_SUCCESS) {
            std::cerr << "Error: build failed for kernel source:" << std::endl;

            print_build_log(program, dev);

            return false;
        } else if (verbose) {
            print_build_log(program, dev);
        }

        kernel = cl::Kernel(program, kernel_name.c_str(), &error);

        HANDLE_CLERR(error);

        return true;
    }
}

bool build_kernels(const cl::Device &dev)
{
    const auto t0 = std::chrono::high_resolution_clock::now();

    bool success = true;

    success &= compile_kernel(blur, dev, "blur", blur_source);

    const auto t1 = std::chrono::high_resolution_clock::now();
    const double t = std::chrono::duration<double>(t1 - t0).count();

    if (verbose) {
        std::cerr << "Building kernels took " << t << " seconds" << std::endl;
    }

    return success;
}
