#include "arguments.hpp"
#include "gpu.hpp"
#include "pnm.hpp"

#include <netpbm/pam.h>

#include <iostream>
#include <thread>

void process_images(void)
{
    const cl::Image2D input = read_pam_to_gpu(in_file);
    const cl::Image2D output = create_output_image();

    set_kernel_args(blur, input, output, 5.0);
    execute_kernel(blur);

    write_pam_to_file(output, out_file);
}

int main(int argc, char **argv)
{
    pm_init(argv[0], 0);

    handle_args(argc, argv);

    if (!gpu_init()) {
        std::cerr << "Error: OpenCL init failed." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    process_images();
}
