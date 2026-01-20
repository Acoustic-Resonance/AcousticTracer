# Communication Standards

## Data from Renderer

In our simulation set normal vector for speaker where sound comes out

The data that needs to be sent to the simulation is as follows:

The GLB file, the area to optimise for
<!-- the direction of the speaker's normal isn't needed as this is found by the optimisation algorithm -->

- the GLB file in it's entirety
- 3 `8bit` numbers:
  - X, Y, and Z dimensions of the selected area
- the material for the floor, walls, and roof
  - *(later)* this will be three different materials for now only one

## Data from Simulation

Sends *five* `8bit` numbers:

- num voxels per dimension (X x Y x Z)
  - don't need size of voxels (the room size is in the `.obj` file)
- num bins in each voxel (only one number)
- unit of time for a bin (measured in number of frames)

Those numbers can be masked 8 bits at a time.

The numbers are then followed by json formatted as such:

```json
{
  "voxel_idx": ["<intensity1>", "<intensity2>", ...]
}
```

Each top level key is the voxel number, and each value is the array of bins.

### Example

```json
{
  "1": [30, 25, ...],
  "2": [...],
  .
  .
  .
  "n": [...]
}
```
