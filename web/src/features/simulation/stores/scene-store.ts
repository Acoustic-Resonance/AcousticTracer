import { create } from "zustand";
import * as THREE from "three";

type RayResponse = Record<string, Record<string, number>[]>;

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
  showGrid: boolean;
  showTexture: boolean;
  pendingFile: File | null;
  gridDimensions: { nx: number; ny: number; nz: number } | null;
  worldDimensions: { x: number; y: number; z: number } | null;
  sourceHasBeenPlaced: boolean;

  setVoxelSize: (size: number) => void;
  setNumRays: (rays: number) => void;
  setFps: (fps: number) => void;
  setBounds: (box: THREE.Box3 | null) => void;
  rayResponse: RayResponse | null;
  frameIndex: number;
  wireframe: boolean;
  setRayResponse: (response: RayResponse) => void;

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
  rayResponse: null,
  sourceHasBeenPlaced: false,
  frameIndex: 0,
  wireframe: false,

  // the actions functions to call when updating state
  setVoxelSize: (size) =>
    set((state) => ({
      config: { ...state.config, voxelSize: size },
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
      sourceHasBeenPlaced: true,
    })),
  setBounds: (box) => {
    if (box) {
      const center = new THREE.Vector3();
      box.getCenter(center);
      set((state) => ({
        bounds: box,
        sourceHasBeenPlaced: true,
        config: {
          ...state.config,
          selectedSource: {
            position: { x: center.x, y: center.y, z: center.z },
            direction: { x: 0, y: 0, z: -1 },
          },
        },
      }));
    } else {
      set({ bounds: null, sourceHasBeenPlaced: false });
    }
  },
  setShowGrid: (visible) => set({ showGrid: visible }),
  setShowTexture: (visible) => set({ showTexture: visible }),
  setPendingFile: (file) =>
    set({
      pendingFile: file,
      // Reset all transient / model-specific state
      sourceHasBeenPlaced: false,
      bounds: null,
      gridDimensions: null,
      worldDimensions: null,
      rayResponse: null,
      showGrid: true,
      config: {
        fileName: file ? file.name : "",
        voxelSize: 2,
        numRays: 10,
        fps: 60,
        material: "Plastic",
        selectedSource: {
          position: { x: 0, y: 0, z: 0 },
          direction: { x: 0, y: 0, z: 0 },
        },
      },
    }),
  setMaterial: (value) =>
    set((state) => ({
      config: {
        ...state.config,
        material: value,
      },
    })),

  setGridDimensions: (dims) => set({ gridDimensions: dims }),
  setWorldDimensions: (dims) => set({ worldDimensions: dims }),
  setRayResponse: (response: RayResponse) => set({ rayResponse: response }),
  setFrameIndex: (i: number) => set({ frameIndex: i }),
  setWireframe: (visible: boolean) => set({ wireframe: visible }),
}));
