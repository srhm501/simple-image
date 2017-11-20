#define MAX_WIDTH 5

__constant float gaussian[MAX_WIDTH*MAX_WIDTH] = {
    0.003765f, 0.015019f, 0.023782f, 0.015019f, 0.003765f,
    0.015019f, 0.059912f, 0.094907f, 0.059912f, 0.015019f,
    0.023792f, 0.094907f, 0.150342f, 0.094907f, 0.023792f,
    0.015019f, 0.059912f, 0.094907f, 0.059912f, 0.015019f,
    0.003765f, 0.015019f, 0.023782f, 0.015019f, 0.002765f
};

const sampler_t sampler =
    CLK_NORMALIZED_COORDS_FALSE |
    CLK_ADDRESS_CLAMP_TO_EDGE |
    CLK_FILTER_NEAREST;

__kernel void blur(__read_only image2d_t input,
                   __write_only image2d_t output,
                   const float width);
{
    const int2 coord = (int2)(get_global_id(0), get_global_id(1));

    int dim = clamp((int)width, 0, MAX_WIDTH)/2;

    float4 conv = 0.0f;
    float norm = 0.0;
    for (int i=-dim; i<=dim; ++i) {
        for (int j=-dim; j<=dim; ++j) {
            const float4 pix = read_imagef(input, sampler, coord + (int2)(i, j));

            const float g = gaussian[(i+MAX_WIDTH/2) * MAX_WIDTH + (j + MAX_WIDTH/2)];

            norm += g;
            conv += g * pix;
        }
    }

    conv /= norm;

    write_imagef(output, coord, conv);
}
