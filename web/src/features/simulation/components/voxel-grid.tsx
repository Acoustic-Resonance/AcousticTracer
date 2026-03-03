import { useRef, useMemo, useLayoutEffect } from "react";
import * as THREE from "three";
import { useSceneStore } from "../stores/scene-store";

const MAX_VOXELS = 500_000;

export default function VoxelGrid() {
  const meshRef = useRef<THREE.InstancedMesh>(null);

  const bounds = useSceneStore((s) => s.bounds);
  const voxelSize = useSceneStore((s) => s.config.voxelSize);
  const visible = useSceneStore((s) => s.showGrid);
  const setGridDims = useSceneStore((s) => s.setGridDimensions);
  const rayResponse = useSceneStore((s) => s.rayResponse);
  const frameIndex = useSceneStore((s) => s.frameIndex);

  // -----------------------------------------------------------------
  // Grid dimensions (always based on effective voxelSize + bounds)
  // -----------------------------------------------------------------
  const gridDims = useMemo(() => {
    if (!bounds) return { nx: 0, ny: 0, nz: 0 };
    const size = new THREE.Vector3();
    bounds.getSize(size);
    return {
      nx: Math.ceil(size.x / voxelSize),
      ny: Math.ceil(size.y / voxelSize),
      nz: Math.ceil(size.z / voxelSize),
    };
  }, [bounds, voxelSize]);

  useLayoutEffect(() => {
    setGridDims(bounds ? gridDims : null);
  }, [bounds, gridDims, setGridDims]);

  // -----------------------------------------------------------------
  // Convert a flat gridIndex → world-space position
  // -----------------------------------------------------------------
  const gridIndexToPos = (idx: number): [number, number, number] => {
    const { nx, ny } = gridDims;
    const z = Math.floor(idx / (nx * ny));
    const rem = idx % (nx * ny);
    const y = Math.floor(rem / nx);
    const x = rem % nx;
    const half = voxelSize / 2;
    return [
      bounds!.min.x + x * voxelSize + half,
      bounds!.min.y + y * voxelSize + half,
      bounds!.min.z + z * voxelSize + half,
    ];
  };

  // -----------------------------------------------------------------
  // Build the list of instances to render
  //   • No ray data  → full white grid
  //   • Ray data     → only voxels with energy in the current frame
  // -----------------------------------------------------------------
  const { instances, isSparse } = useMemo(() => {
    // --- Sparse: frame data present ---
    if (rayResponse && bounds) {
      const frame = rayResponse[`frame_${frameIndex}`] ?? [];
      const voxels = frame.map((bin) => {
        const [gridIndex, intensity] = Object.entries(bin)[0] as [
          string,
          number,
        ];
        return { gridIndex: Number(gridIndex), intensity };
      });
      return { instances: voxels, isSparse: true };
    }

    // --- Full white grid ---
    if (!bounds) return { instances: [], isSparse: false };
    const { nx, ny, nz } = gridDims;
    const total = nx * ny * nz;
    if (total > MAX_VOXELS) return { instances: [], isSparse: false };

    const voxels = Array.from({ length: total }, (_, i) => ({
      gridIndex: i,
      intensity: -1, // sentinel → white
    }));
    return { instances: voxels, isSparse: false };
  }, [rayResponse, frameIndex, bounds, gridDims]);

  // -----------------------------------------------------------------
  // Write instance matrices (positions)
  // -----------------------------------------------------------------
  useLayoutEffect(() => {
    if (!meshRef.current || !bounds || instances.length === 0) return;
    const arr = meshRef.current.instanceMatrix.array as Float32Array;

    instances.forEach(({ gridIndex }, i) => {
      const [px, py, pz] = gridIndexToPos(gridIndex);
      const off = i * 16;
      arr[off + 0] = 1;
      arr[off + 5] = 1;
      arr[off + 10] = 1; // scale
      arr[off + 12] = px;
      arr[off + 13] = py;
      arr[off + 14] = pz;
      arr[off + 15] = 1;
    });

    meshRef.current.instanceMatrix.needsUpdate = true;
  }, [instances, bounds, voxelSize]);

  // -----------------------------------------------------------------
  // Write instance colours (sparse mode only)
  // -----------------------------------------------------------------
  useLayoutEffect(() => {
    if (!meshRef.current || !isSparse || instances.length === 0) return;

    if (!meshRef.current.instanceColor) {
      meshRef.current.setColorAt(0, new THREE.Color());
    }

    const color = new THREE.Color();
    instances.forEach(({ intensity }, i) => {
      const hue = THREE.MathUtils.mapLinear(intensity, 0, 1, 0.66, 1);
      color.setRGB(hue, 0, 0);
      meshRef.current!.setColorAt(i, color);
    });

    meshRef.current.instanceColor!.needsUpdate = true;
  }, [instances, isSparse]);

  if (!bounds) return null;

  return (
    <instancedMesh
      ref={meshRef}
      args={[undefined, undefined, instances.length]}
      visible={visible}
    >
      <boxGeometry args={[voxelSize, voxelSize, voxelSize]} />
      <meshStandardMaterial
        color="#ffffff"
        transparent
        opacity={isSparse ? 0.85 : 0.15}
        depthWrite={false}
      />
    </instancedMesh>
  );
}
