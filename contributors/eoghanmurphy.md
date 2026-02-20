# Eoghan Murphy Log Book

## Week 2

- Defined the communication between the simulation backend and the rendering frontend.
- Built the first simulated 3D room files for testing purposes.
    - This involved modelling simple rooms inside Blender and exporting their respective files to then be used by the renderer and simulation.
- Wrote the documentation for the public headers on our simulation library.
    - Set up `doxygen` (a documentation library) and a GitHub pages so our documented code can be read through without the source code.

## Week 3

- Implemented the physics behind elastic ray reflections.
- Implemented AABB (Axis Aligned Boundary Box) functionality for a given triangle(s).
    - This involves calculating the minimum and maximum points of a set of points, essentially creating a rough hit-box.
- Started building the BVH (Bounding Volume Hierarchy) implementation.
    - This is a binary tree used for optimising collision detection from `O(n)`, where `n` is the number of triangles to `O(n log n)`.
    - This involved researching potential BVH algorithms, and weighing pros and cons of each to decide on a suitable implementation.

## Week 4

- Continued my implementation of a BVH binary tree.
    - I spent last week research potential implementations before deciding on the BONSAI variant.
    - I then spent this week implementing the second step of the five step algorithm.
    - This involved subdividing the AABB surrounding the entire scene into smaller and smaller AABB containing groups of triangles, until the group was of a predetermined size N.
- I refactored the ray-triangle intersection code to first check if it intersects with a group and then relevant triangles, increasing our performance from 20±5 rays/s to 1000±200 rays/s.
- I further refactored our definition of a vector to allow for easier threading when I later implement that.

## Week 5

- Fixed issue with models causing rays to clip out of bounds.
- Implemented a variant of Radix sort for sorting negative and positive floats by treating them as positive integers.
- Wrote function declarations for BVH construction helpers.

## Week 6

- Continued with the BVH implementation.
- Implemented surface area for AABBs which will be later used to optimise the scene's BVH.
- Implemented a variation of Radix sort for sorting an array of mixed negative and positive floats.
- Implemented a stable partition algorithm to reduce the need for continuously re-sorting the array after splits.
- Implemented an algorithm for finding the optimal place to split a node so that it's children are balanced.