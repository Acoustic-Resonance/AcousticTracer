/**
 * ATRB binary format parser for simulation results.
 *
 * Binary layout (little-endian):
 *   Header (16 B):  "ATRB" magic | numFrames u32 | numVoxels u32 | reserved u32
 *   Frame table:    numFrames × (offset u32, count u32)
 *   Frame data:     per frame SoA: indices[count] u32 then energies[count] f32
 *
 * Parsing creates zero-copy typed-array views into the original ArrayBuffer.
 */

/** One frame of sparse voxel energy data (zero-copy views). */
export interface RayFrame {
  indices: Uint32Array;
  energies: Float32Array;
}

/** Parse an ATRB binary result buffer into frames. */
export function parseResultBuffer(buffer: ArrayBuffer): RayFrame[] {
  const view = new DataView(buffer);
  const numFrames = view.getUint32(4, true);
  const frames: RayFrame[] = new Array(numFrames);

  for (let f = 0; f < numFrames; f++) {
    const tableEntry = 16 + f * 8;
    const offset = view.getUint32(tableEntry, true);
    const count = view.getUint32(tableEntry + 4, true);

    // Zero-copy views directly into the ArrayBuffer
    frames[f] = {
      indices: new Uint32Array(buffer, offset, count),
      energies: new Float32Array(buffer, offset + count * 4, count),
    };
  }

  return frames;
}
