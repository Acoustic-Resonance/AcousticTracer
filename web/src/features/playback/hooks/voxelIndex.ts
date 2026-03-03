export function indexToCoord(
  index: number,
  dims: { nx: number; ny: number; nz: number },
) {
  const z = Math.floor(index / (dims.nx * dims.ny));
  const y = Math.floor((index % (dims.nx * dims.ny)) / dims.nx);
  const x = index % dims.nx;
  return { x, y, z };
}

export function coordToIndex(
  x: number,
  y: number,
  z: number,
  dims: { nx: number; ny: number; nz: number },
) {
  return z * dims.nx * dims.ny + y * dims.nx + x;
}
