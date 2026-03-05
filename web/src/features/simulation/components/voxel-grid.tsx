import { useRef, useMemo, useLayoutEffect, useCallback } from "react";
import * as THREE from "three";
import { useSceneStore } from "../stores/scene-store";
import { useRayResponse } from "../api/use-simulation-hooks";
import type { RayFrame } from "../api/parse-result-binary";

const MAX_VOXELS = 500_000;
const ZERO_MATRIX = new THREE.Matrix4().makeScale(0, 0, 0);
const WHITE = new THREE.Color("#ffffff");

export default function VoxelGrid() {
  const meshRef = useRef<THREE.InstancedMesh>(null);

  const bounds = useSceneStore((s) => s.bounds);
  const voxelSize = useSceneStore((s) => s.config.voxelSize);
  const visible = useSceneStore((s) => s.showGrid);
  const setGridDims = useSceneStore((s) => s.setGridDimensions);
  const resultFileId = useSceneStore((s) => s.resultFileId);
  const frameIndex = useSceneStore((s) => s.frameIndex);
  const numRays = useSceneStore((s) => s.config.numRays);

  // Ray response data fetched/cached entirely by TanStack Query.
  // Now returns RayFrame[] (typed arrays) instead of JSON objects.
  const { data: rayResponse } = useRayResponse(resultFileId ?? undefined);

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
  // Convert a flat gridIndex → world-space position (memoised)
  // -----------------------------------------------------------------
  const gridIndexToPos = useCallback(
    (idx: number): [number, number, number] => {
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
    },
    [gridDims, voxelSize, bounds],
  );

  // -----------------------------------------------------------------
  // Current frame data (null when no simulation results → full grid)
  // -----------------------------------------------------------------
  const currentFrame: RayFrame | null = useMemo(() => {
    if (!rayResponse || !bounds) return null;
    return rayResponse[frameIndex] ?? null;
  }, [rayResponse, frameIndex, bounds]);

  const isSparse = currentFrame !== null;

  // -----------------------------------------------------------------
  // Full-grid count (white grid shown before simulation runs)
  // -----------------------------------------------------------------
  const fullGridCount = useMemo(() => {
    if (rayResponse || !bounds) return 0;
    const { nx, ny, nz } = gridDims;
    const total = nx * ny * nz;
    return total > MAX_VOXELS ? 0 : total;
  }, [rayResponse, bounds, gridDims]);

  const activeCount = currentFrame
    ? currentFrame.indices.length
    : fullGridCount;

  // -----------------------------------------------------------------
  // Pre-compute the max instance count across ALL frames so the
  // InstancedMesh is allocated once and never remounted.
  // -----------------------------------------------------------------
  const maxInstanceCount = useMemo(() => {
    if (rayResponse && bounds) {
      let max = 0;
      for (const frame of rayResponse) {
        if (frame.indices.length > max) max = frame.indices.length;
      }
      return Math.max(max, 1);
    }
    return fullGridCount > 0 ? fullGridCount : 0;
  }, [rayResponse, bounds, fullGridCount]);

  // -----------------------------------------------------------------
  // Write instance matrices — position active voxels, zero-scale the rest
  // -----------------------------------------------------------------
  useLayoutEffect(() => {
    const mesh = meshRef.current;
    if (!mesh || !bounds || maxInstanceCount === 0) return;

    const mat = new THREE.Matrix4();

    if (currentFrame) {
      // Sparse mode: read voxel indices directly from typed array
      const { indices } = currentFrame;
      for (let i = 0; i < indices.length; i++) {
        const [px, py, pz] = gridIndexToPos(indices[i]);
        mat.makeTranslation(px, py, pz);
        mesh.setMatrixAt(i, mat);
      }
      for (let i = indices.length; i < maxInstanceCount; i++) {
        mesh.setMatrixAt(i, ZERO_MATRIX);
      }
    } else {
      // Full grid mode: all voxels visible
      for (let i = 0; i < activeCount; i++) {
        const [px, py, pz] = gridIndexToPos(i);
        mat.makeTranslation(px, py, pz);
        mesh.setMatrixAt(i, mat);
      }
      for (let i = activeCount; i < maxInstanceCount; i++) {
        mesh.setMatrixAt(i, ZERO_MATRIX);
      }
    }

    mesh.instanceMatrix.needsUpdate = true;
    mesh.count = maxInstanceCount;
  }, [
    currentFrame,
    activeCount,
    bounds,
    voxelSize,
    maxInstanceCount,
    gridIndexToPos,
  ]);

  // -----------------------------------------------------------------
  // Write instance colours
  // -----------------------------------------------------------------
  useLayoutEffect(() => {
    const mesh = meshRef.current;
    if (!mesh || maxInstanceCount === 0) return;

    // Ensure the instanceColor buffer exists
    if (!mesh.instanceColor) {
      mesh.setColorAt(0, WHITE);
    }

    if (currentFrame) {
      // Sparse: read energies directly from typed array
      const { energies } = currentFrame;
      const color = new THREE.Color();
      for (let i = 0; i < energies.length; i++) {
        const hue = THREE.MathUtils.mapLinear(
          energies[i] * numRays,
          0,
          1,
          0.66,
          1,
        );
        color.setHSL(hue, 1, 0.5);
        mesh.setColorAt(i, color);
      }
    } else {
      // Full grid — all white
      for (let i = 0; i < activeCount; i++) {
        mesh.setColorAt(i, WHITE);
      }
    }

    mesh.instanceColor!.needsUpdate = true;
  }, [currentFrame, activeCount, maxInstanceCount]);

  if (!bounds || maxInstanceCount === 0) return null;

  return (
    <instancedMesh
      ref={meshRef}
      args={[undefined, undefined, maxInstanceCount]}
      visible={visible}
      frustumCulled={false}
    >
      <boxGeometry args={[voxelSize, voxelSize, voxelSize]} />
      <meshStandardMaterial
        color="#ffffff"
        transparent
        opacity={isSparse ? 0.85 : 0.15}
        depthWrite={true}
      />
    </instancedMesh>
  );
}
