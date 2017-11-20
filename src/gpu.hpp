#ifndef GPU_HPP_
#define GPU_HPP_

#include <CL/cl.hpp>

#include <string>

extern cl::Kernel blur;

extern cl::Context context;
extern cl::CommandQueue queue;

bool gpu_init(void);

cl::Image2D create_output_image(void);

void set_kernel_args(cl::Kernel &kernel,
                     const cl::Image2D &input,
                     const cl::Image2D &output,
                     float param);

void execute_kernel(const cl::Kernel &kernel);

#endif // GPU_HPP_
