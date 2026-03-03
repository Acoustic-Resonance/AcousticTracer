import { useRef, useMemo, useLayoutEffect, useState, useEffect } from "react";
import * as THREE from "three";
import { useSceneStore } from "../stores/scene-store";
import { useThree } from "@react-three/fiber";

const MAX_VOXELS = 500_000;
const DEBOUNCE_MS = 150;

export default function VoxelGrid() {
  const meshRef = useRef<THREE.InstancedMesh>(null);

  // Subscribe to the data this component needs
  const bounds = useSceneStore((state) => state.bounds);
  const storeVoxelSize = useSceneStore((state) => state.config.voxelSize);
  const visible = useSceneStore((state) => state.showGrid);
  const setGridDimensions = useSceneStore((state) => state.setGridDimensions);
  const setWorldDimensions = useSceneStore((state) => state.setWorldDimensions);
  // Debounce voxelSize so the grid only rebuilds after the user stops dragging
  const [voxelSize, setVoxelSize] = useState(storeVoxelSize);
  useEffect(() => {
    const id = setTimeout(() => setVoxelSize(storeVoxelSize), DEBOUNCE_MS);
    return () => clearTimeout(id);
  }, [storeVoxelSize]);

  const setSelectedSource = useSceneStore((state) => state.setSelectedSource);
  const rayTracerData = useSceneStore((state) => state.rayResponse);
  const frameIndex = useSceneStore((state) => state.frameIndex);
  const { camera } = useThree();

  const { count, gridDims } = useMemo(() => {
    if (!bounds) {
      return { count: 0, gridDims: { nx: 0, ny: 0, nz: 0 } };
    }

    const size = new THREE.Vector3();
    bounds.getSize(size);

    const nx = Math.ceil(size.x / voxelSize);
    const ny = Math.ceil(size.y / voxelSize);
    const nz = Math.ceil(size.z / voxelSize);
    const total = nx * ny * nz;
    const dims = { nx, ny, nz };

    // Safety cap: skip rendering if too many voxels
    if (total > MAX_VOXELS) {
      return { count: 0, gridDims: dims };
    }

    return {
      count: total,
      gridDims: dims,
    };
  }, [bounds, voxelSize]);

  // Update world dimensions only when bounds change (NOT when voxelSize changes)
  useLayoutEffect(() => {
    if (!bounds) {
      setWorldDimensions(null);
      return;
    }
    const size = new THREE.Vector3();
    bounds.getSize(size);
    setWorldDimensions({ x: size.x, y: size.y, z: size.z });
  }, [bounds, setWorldDimensions]);

  // Update grid dimensions when gridDims change
  useLayoutEffect(() => {
    if (!bounds) {
      setGridDimensions(null);
      return;
    }
    setGridDimensions(gridDims);
  }, [bounds, gridDims, setGridDimensions]);

  useLayoutEffect(() => {
    if (!meshRef.current || !bounds) return;

    const array = meshRef.current.instanceMatrix.array as Float32Array;
    const halfSize = voxelSize / 2;
    const scale = 1; // Slight gap between voxels
    const { nx, ny, nz } = gridDims;

    let i = 0;
    for (let z = 0; z < nz; z++) {
      for (let y = 0; y < ny; y++) {
        for (let x = 0; x < nx; x++) {
          const posX = bounds.min.x + x * voxelSize + halfSize;
          const posY = bounds.min.y + y * voxelSize + halfSize;
          const posZ = bounds.min.z + z * voxelSize + halfSize;

          const offset = i * 16;

          // Write matrix directly to GPU buffer (Column-major order)
          array[offset + 0] = scale; // Scale X
          array[offset + 5] = scale; // Scale Y
          array[offset + 10] = scale; // Scale Z
          array[offset + 12] = posX; // Position X
          array[offset + 13] = posY; // Position Y
          array[offset + 14] = posZ; // Position Z
          array[offset + 15] = 1; // Homogeneous coordinates

          i++;
        }
      }
    }
    meshRef.current.instanceMatrix.needsUpdate = true;
  }, [bounds, voxelSize, gridDims, count]);

  useLayoutEffect(() => {
    if (!meshRef.current || !rayTracerData) return;

    if (!meshRef.current.instanceColor) {
      meshRef.current.setColorAt(0, new THREE.Color());
    }

    const color = new THREE.Color();

    // sets white
    for (let i = 0; i < count; i++) {
      meshRef.current.setColorAt(i, new THREE.Color("white"));
    }

    const frameKey = `frame_${frameIndex}`;
    const frame = rayTracerData[frameKey];

    if (frame) {
      frame.forEach((bin) => {
        const [voxelIndex, intensity] = Object.entries(bin)[0];
        const hue = THREE.MathUtils.mapLinear(intensity, 0, 1, 0.66, 1);
        color.setRGB(hue, 0, 0);
        meshRef.current!.setColorAt(Number(voxelIndex), color);
      });
    }

    meshRef.current.instanceColor!.needsUpdate = true;
  }, [rayTracerData, frameIndex, count]);

  // Compute voxel center from instance index
  const handlePick = (e: any) => {
    // don't allow the user to change the source position once they have run the simulation
    if (rayTracerData) return;
    // Stop from selecting multiple voxels
    e.stopPropagation();

    // The id of the voxel
    const instanceId = e.instanceId as number | null;
    // The point I click with mouse
    const point = e.point as THREE.Vector3 | undefined;
    console.log("voxel-grid pick:", { instanceId, point });
    if (instanceId == null || !bounds) return;

    try {
      // Creating a 4x4 matrix for the voxel i selected
      const mat = new THREE.Matrix4();
      // Reading in the information about the voxel based on instanceId into mat
      meshRef.current!.getMatrixAt(instanceId, mat);
      // the voxel position i selected relative to the instance mesh
      const posLocal = new THREE.Vector3();
      // the rotation angle of the voxel i selected
      const quat = new THREE.Quaternion();
      // scale of the voxel
      const scale = new THREE.Vector3();
      // normal of the camera (direction)
      const direction = new THREE.Vector3();
      camera.getWorldDirection(direction);

      // Decomposing info into the variables to be used
      mat.decompose(posLocal, quat, scale);

      // Convert to world position (the actual coords)
      meshRef.current!.updateMatrixWorld();
      const posWorld = posLocal.applyMatrix4(meshRef.current!.matrixWorld);

      // Clamping to bounds so cant go outside bounding box
      const clamp = (v: number, a: number, b: number) =>
        Math.max(a, Math.min(b, v));
      const sx = clamp(posWorld.x, bounds.min.x, bounds.max.x);
      const sy = clamp(posWorld.y, bounds.min.y, bounds.max.y);
      const sz = clamp(posWorld.z, bounds.min.z, bounds.max.z);

      setSelectedSource(
        { x: sx, y: sy, z: sz },
        { x: direction.x, y: direction.y, z: direction.z },
      );
      console.log(
        "Updated store value:",
        useSceneStore.getState().config.selectedSource,
      );
      return;
    } catch (err) {
      // If getMatrixAt fails, do nothing — we only accept picks backed by instance matrices
      console.warn("voxel-grid: getMatrixAt failed — ignoring pick", err);
      return;
    }
  };

  if (!bounds) return null;

  return (
    <>
      <instancedMesh
        ref={meshRef}
        args={[undefined, undefined, count]}
        visible={visible}
      >
        <boxGeometry args={[voxelSize, voxelSize, voxelSize]} />
        <meshStandardMaterial
          color="#ffffff"
          transparent
          opacity={0.1}
          depthWrite={false}
        />
      </instancedMesh>
    </>
  );
}
