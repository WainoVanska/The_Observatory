# Week 11 (13.7-19.7.2026)

## I learned:

- how instancing works with models, created a group of asteroids circling a planet to test this out.

<img src="../learning_log_images/instancing_asteroids.png" alt="-" width="500">

## I struggled:

- with implementing the asteroids with the instancing method. First I didn't see the asteroids at all due to trying to implement the changes to "model_fragment.glsl" instead of making a separate shader for the instanced rocks, I wanted to try to make the shader usage more scalable but it didn't work since my model didn't have needed information on the textures (rock model didn't have texture maps). I also tried a few different debugging tactics like removing face culling, but this didn't help either. By using a simple fragment shader I realized that the asteroid creation is working otherwise except for the fragment shader. The fix was to create a fragment shader that didn't deal with texture mapping but only used color based material (however I think there's probably an easier way to get a scalable result on the model_fragment.glsl instead of creating a new one)


## Thoughts:

- It was really cool to test increasing the asteroid count and tracking fps performance. After increasing the count to 150k I started noticing drops below 100fps (normally fps stays in the 180fps area).