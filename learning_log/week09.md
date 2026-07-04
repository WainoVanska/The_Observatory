# Week 9 (29.6-5.7.2026)

## I learned:

- to create a blurring effect to the scene through separate framebuffer post-processing



<img src="../learning_log_images/depth_testing_debug.png" alt="depth test debugger" width="500">


## I struggled:

- with blurring effect implementation through framebuffer post-processing. I followed the OpenGL guide, but took quite a lot of time to figure out why instead of blurring I only got a white screen when pressing B. In the end the issue was the order of usage of shader programs, in the init function of the scene I accidentally added the screen shader function in between model shader functions, which created the incorrect pipeline.

## Thoughts:

