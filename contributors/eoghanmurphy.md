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
