import { create } from "zustand";
import * as THREE from "three";

interface SceneState {
  config: {
    fileName: string;
    voxelSize: number;
    numRays: number;
    fps: number;
    material: string;
    selectedSource: {
      position: {
        x: number;
        y: number;
        z: number;
      };
      direction: {
        x: number;
        y: number;
        z: number;
      };
    };
  };
  bounds: THREE.Box3 | null;
  num_voxels: number;
  showGrid: boolean;
  showTexture: boolean;
  pendingFile: File | null;
  gridDimensions: { nx: number; ny: number; nz: number } | null;
  worldDimensions: { x: number; y: number; z: number } | null;
  voxelCount: number | null;

  setVoxelSize: (size: number) => void;
  setNumRays: (rays: number) => void;
  setFps: (fps: number) => void;
  setBounds: (box: THREE.Box3 | null) => void;
  resultFileId: string | null;
  frameIndex: number;
  wireframe: boolean;

  setNumVoxels: (n: number) => void;
  setResultFileId: (id: string | null) => void;
  setShowGrid: (visible: boolean) => void;
  setShowTexture: (visible: boolean) => void;
  setPendingFile: (file: File | null) => void;
  setMaterial: (value: string) => void;
  setGridDimensions: (
    dims: { nx: number; ny: number; nz: number } | null,
  ) => void;
  setWorldDimensions: (
    dims: { x: number; y: number; z: number } | null,
  ) => void;
  setVoxelCount: (count: number | null) => void;
  setSelectedSource: (
    dims: { x: number; y: number; z: number },
    direction: { x: number; y: number; z: number },
  ) => void;
  setFrameIndex: (i: number) => void;
  setWireframe: (visible: boolean) => void;
}

export const useSceneStore = create<SceneState>()((set) => ({
  config: {
    fileName: "",
    voxelSize: 2,
    numRays: 10,
    fps: 60,
    material: "Plastic",
    selectedSource: {
      position: {
        x: 0,
        y: 0,
        z: 0,
      },
      direction: {
        x: 0,
        y: 0,
        z: 0,
      },
    },
  },
  bounds: null,
  showGrid: true,
  showTexture: true,
  pendingFile: null,
  gridDimensions: null,
  worldDimensions: null,
  voxelCount: null,
  resultFileId: null,
  frameIndex: 0,
  wireframe: false,
  num_voxels: 0,

  // the actions functions to call when updating state
  setVoxelSize: (size) =>
    set((state) => ({
      config: { ...state.config, voxelSize: Math.round(size * 100) / 100 },
    })),
  setNumRays: (rays) =>
    set((state) => ({
      config: { ...state.config, numRays: rays },
    })),
  setFps: (fps) =>
    set((state) => ({
      config: { ...state.config, fps: fps },
    })),
  setSelectedSource: (position, direction) =>
    set((state) => ({
      config: { ...state.config, selectedSource: { position, direction } },
    })),
  setBounds: (box) => {
    if (box) {
      const center = new THREE.Vector3();
      box.getCenter(center);
      const size = new THREE.Vector3();
      box.getSize(size);
      set((state) => ({
        bounds: box,
        worldDimensions: { x: size.x, y: size.y, z: size.z },
        config: {
          ...state.config,
          selectedSource: {
            position: { x: center.x, y: center.y, z: center.z },
            direction: { x: 0, y: 0, z: -1 },
          },
        },
      }));
    } else {
      set({ bounds: null, worldDimensions: null });
    }
  },
  setShowGrid: (visible) => set({ showGrid: visible }),
  setShowTexture: (visible) => set({ showTexture: visible }),
  setPendingFile: (file) => {
    if (file) {
      // New file selected — full reset so the scene starts fresh
      set({
        pendingFile: file,
        bounds: null,
        gridDimensions: null,
        worldDimensions: null,
        voxelCount: null,
        resultFileId: null,
        showGrid: true,
        showTexture: true,
        wireframe: false,
        frameIndex: 0,
        config: {
          fileName: file.name,
          voxelSize: 2,
          numRays: 10,
          fps: 60,
          material: "Plastic",
          selectedSource: {
            position: { x: 0, y: 0, z: 0 },
            direction: { x: 0, y: 0, z: 0 },
          },
        },
      });
    } else {
      // Clearing pendingFile only (e.g. when loading a saved simulation)
      set({ pendingFile: null });
    }
  },
  setMaterial: (value) =>
    set((state) => ({
      config: {
        ...state.config,
        material: value,
      },
    })),

  setNumVoxels: (n: number) => set({ num_voxels: n }),
  setGridDimensions: (dims) => set({ gridDimensions: dims }),
  setWorldDimensions: (dims) => set({ worldDimensions: dims }),
  setVoxelCount: (count) => set({ voxelCount: count }),
  setResultFileId: (id: string | null) => set({ resultFileId: id }),
  setFrameIndex: (i: number) => set({ frameIndex: i }),
  setWireframe: (visible: boolean) => set({ wireframe: visible }),
}));
