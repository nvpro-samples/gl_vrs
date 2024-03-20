# Variable Rate Shading

![Two screenshots of this sample side by side. On the left, the sample renders 1000 tori. The shading rate for the blue tori decreases as the distance to the center increases; in the periphery, they are not rendered at all. The green tori, on the other hand, are always rendered at full (1x1) shading rate. On the right, the sample shows the shading rate per pixel.](doc/gl_vrs.png)

## This sample shows the functionality of Variable Rate Shading

Variable Rate Shading is a hardware feature introduced with the NVIDIA Turing GPUs and exposed via the GL_NV_shading_rate_image extension.

Variable Rate Shading allows the hardware to shade primitives at a different frequency than the rate of rasterization. The user can pick between various rates, including no shading, normal resolution, half resolution and quarter resolution. If the rendering uses multi sampling, the shading rate can be increased to allow for multiple samples within the same primitive to be shaded. This can reduce shader generated aliasing. The shading rate is defined for blocks of pixels with a constant rate within each block.


## Sample showing a reduced shading rate

The sample lets the user pick predefined shading rates. Checking "visualizeShadingRate" will show a color-coded image of the shading rate per pixel.

It is possible to vary the shading rate per triangle in the vertex shader; in the sample, all green objects are selected for full shading rate. This can be deactivated from the menu.

As the reduction in shading rate can be subtle, the sample allows rendering at a lower resolution and "zooming in" via the "framebuffer scaling" setting.


#### Building
Ideally, clone this and other interesting [nvpro-samples](https://github.com/nvpro-samples) repositories into a common subdirectory. You will always need [nvpro_core](https://github.com/nvpro-samples/nvpro_core). The nvpro_core is searched either as a subdirectory of the sample, or one directory up.

If you are interested in multiple samples, you can use the [build_all](https://github.com/nvpro-samples/build_all) CMake as the entry point; it will also give you options to enable/disable individual samples when creating the solution.
