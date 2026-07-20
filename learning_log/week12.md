# Week 11 (20.7-26.7.2026)

## I learned:

- how point shadows work. Created this by making a depth cubemap containing six faces of the room scene that each stored depth data(distance from the the point light to the closest surface). Instead of orthographic projection with directional lighting, this time I used perspective projection for each of the six faces.

<img src="../learning_log_images/jagged_shadow_edges.png" alt="-" width="500">
<img src="../learning_log_images/jagged_edge_pcf.png" alt="-" width="500">

## I struggled:

- with removing jagged shadow edges. First I started using PCF (percentage-closer filtering) with the LearnOpenGL tutorial (Point Shadows chapter), but ended up accidentally producing shadow acne as a result. The reason for this was that I was accidentally also drawing shadows for the wall itself, even though drawLitObjects() was already doing them (in a different way). By removing the drawClosedRoom() function from the drawPointShadowCasters() everything started looking better. I also had to play around the with the point shadow bias and disk radius to get the best fit for the room mode. 

- The sphere had also issues with self-shadowing, it became darker after implementing the point shadows. The fix to this was to discard the front faces of the sphere in the shadow rendering part and only render things from the back faces, playing with face culling functions really worked on this issue.

## Thoughts:
