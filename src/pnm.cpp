#include "arguments.hpp"
#include "clerr.hpp"
#include "gpu.hpp"

#include <CL/cl.hpp>
#include <netpbm/pam.h>

#include <cassert>
#include <iostream>

size_t image_width = 0;
size_t image_height = 0;
size_t image_depth = 0;

struct pam pam_struct;

cl::size_t<3> origin;
cl::size_t<3> region;

cl::Image2D read_pam_to_gpu(const std::string &filename)
{
    if (verbose) {
        std::cerr << "Reading image" << std::endl;
    }

    tuple **image;
    if (filename.empty()) {
        image = pnm_readpam(stdin, &pam_struct, PAM_STRUCT_SIZE(tuple_type));
    } else {
        FILE *file = fopen(filename.c_str(), "r");
        if (!file) {
            std::cerr << "Error opening file " << filename << std::endl;
            std::exit(EXIT_FAILURE);
        }

        image = pnm_readpam(file, &pam_struct, PAM_STRUCT_SIZE(tuple_type));

        fclose(file);
    }

    image_width = pam_struct.width;
    image_height = pam_struct.height;
    image_depth = pam_struct.depth;

    region[0] = image_width;
    region[1] = image_height;
    region[2] = 1;

    assert(image_depth == 1 || image_depth == 3);

    if (verbose) {
        std::cerr << "-- Image width:  " << image_width  << std::endl;
        std::cerr << "-- Image height: " << image_height << std::endl;
        std::cerr << "-- Image depth:  " << image_depth  << std::endl;

        std::cerr << "-- Total memory: " << image_width * image_height * 4 * sizeof (cl_float) << " bytes" << std::endl;
    }

    std::vector<cl_float> host_data(image_width * image_height * 4, 1.0f);

    const cl_float imaxval = 1.0f / pam_struct.maxval;

    for (size_t i=0; i<image_height; ++i) {
        for (size_t j=0; j<image_width; ++j) {
            for (size_t k=0; k<image_depth; ++k) {
                const unsigned idx = 4 * (i * image_width + j) + k;
                host_data[idx] = image[i][j][k] * imaxval;

                if (image_depth == 3) {
                    host_data[idx + 1] = 1.0f; // alpha channel
                }
            }
        }
    }

    pnm_freepamarray(image, &pam_struct);

    const cl_mem_flags flags = CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY;

    const cl::ImageFormat format(CL_RGBA, CL_FLOAT);

    if (verbose) {
        std::cerr << "Sending image data to device" << std::endl;
    }

    cl_int error;
    cl::Image2D dev_image(context, flags, format, image_width, image_height, 0, nullptr, &error);
    HANDLE_CLERR(error);


    HANDLE_CLERR(queue.enqueueWriteImage(dev_image,
                                         CL_TRUE,
                                         origin,
                                         region,
                                         0,
                                         0,
                                         host_data.data(),
                                         nullptr,
                                         nullptr));

    return dev_image;
}

void write_pam_to_file(const cl::Image2D &output, const std::string &filename)
{
    if (verbose) {
        std::cerr << "Writing image to file" << std::endl;
    }

    tuple **image = pnm_allocpamarray(&pam_struct);

    std::vector<cl_float> host_data(image_width * image_height * 4);

    HANDLE_CLERR(queue.enqueueReadImage(output,
                                        CL_TRUE,
                                        origin,
                                        region,
                                        4 * sizeof (cl_float) * image_width,
                                        0,
                                        host_data.data(),
                                        nullptr,
                                        nullptr));

    for (size_t i=0; i<image_height; ++i) {
        for (size_t j=0; j<image_width; ++j) {
            for (size_t k=0; k<image_depth; ++k) {
                image[i][j][k] = host_data[4 * (i * image_width + j) + k] * pam_struct.maxval;
            }
        }
    }

    if (filename.empty()) {
        pam_struct.file = stdout;
        pnm_writepam(&pam_struct, image);
    } else {
        FILE *file = fopen(filename.c_str(), "w");
        if (!file) {
            std::cerr << "Error opening file " << filename << std::endl;
            std::exit(EXIT_FAILURE);
        }

        pam_struct.file = file;
        pnm_writepam(&pam_struct, image);
        fclose(file);
    }

    pnm_freepamarray(image, &pam_struct);
}
