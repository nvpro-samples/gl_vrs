# Variable Rate Shading

## These samples show the functionality of Variable Rate Shading

Variable Rate Shading is a hardware feature introduced with the NVIDIA Turing GPUs and exposed via the GL_NV_shading_rate_image extension.

Variable Rate Shading allows the hardware to shade primitives at a different frequency than the rate of rasterization. The user can pick between various rates, including no shading, normal resolution, half resolution and quarter resolution. If the rendering uses multi sampling, the shading rate can be increased to allow for multiple sample within the same primitive to be shaded. This can reduce shader generated aliasing. The shading rate is defined for blocks of pixels with a constant rate within each block.


## Sample showing a reduced shading rate

The sample lets the user pick predefined shading rates. The shading rate can get visualized by color coding from the menu.

It is possible to vary the shading rate per triangle in the vertex shader, in the sample all green objects are selected for full shading rate. This can get deactivated from the menu.

As the reduction in shading rate can be subtle, the sample allows to render at a lower resolution and "zoom in" via the "framebuffer scaling" setting.


#### Building
Ideally, clone this and other interesting [nvpro-samples](https://github.com/nvpro-samples) repositories into a common subdirectory. You will always need [nvpro_core](https://github.com/nvpro-samples/nvpro_core). The nvpro_core is searched either as a subdirectory of the sample, or one directory up.

If you are interested in multiple samples, you can use [build_all](https://github.com/nvpro-samples/build_all) CMAKE as entry point, it will also give you options to enable/disable individual samples when creating the solutions.
