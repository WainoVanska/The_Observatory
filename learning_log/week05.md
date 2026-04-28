# Week 5 (27.4-3.5.2026)

## I learned:

- how to move the view with WASD and mouse by changing the camera pos and camera dir. vectors in the view matrix (free roam camera)
- how to organize multiple objects by making separate draw functions for them / making separate files to keep main.cpp for cluttering

## I struggled:

- differences between the camera position and the direction of the camera
- getting the camera pos working relative to all camera directions when moving with WASD and mouse together
    - yaw and pitch (hard to figure out first why the cosines and sines were being multiplied for horizontal movement)