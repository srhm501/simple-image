#ifndef PNM_HPP
#define PNM_HPP

#include <CL/cl.hpp>

extern size_t image_width;
extern size_t image_height;
extern size_t image_depth;

extern cl::size_t<3> origin;
extern cl::size_t<3> region;

cl::Image2D read_pam_to_gpu(const std::string &filename);

void write_pam_to_file(const cl::Image2D &output, const std::string &filename);

#endif // PNM_HPP
