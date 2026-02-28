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
  pendingFile: File | null;
  gridDimensions: { nx: number; ny: number; nz: number } | null;
  worldDimensions: { x: number; y: number; z: number } | null;
  rayResponse: RayResponse | null;
  frameIndex: number;

  setVoxelSize: (size: number) => void;
  setRayResponse: (response: RayResponse) => void;
  setBounds: (box: THREE.Box3) => void;
  setShowGrid: (visible: boolean) => void;
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
}

export const useSceneStore = create<SceneState>()((set, get) => ({
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
  rawBounds: null,
  showGrid: true,
  pendingFile: null,
  gridDimensions: null,
  worldDimensions: null,
  rayResponse: null,
  frameIndex: 0,

  // the actions functions to call when updating state
  setVoxelSize: (size) =>
    set((state) => ({
      config: { ...state.config, voxelSize: size },
    })),
  setSelectedSource: (position, direction) =>
    set((state) => ({
      config: { ...state.config, selectedSource: { position, direction } },
    })),
  setBounds: (box) => set({ bounds: box }),
  setShowGrid: (visible) => set({ showGrid: visible }),
  setPendingFile: (file) =>
    set((state) => ({
      pendingFile: file,
      config: {
        ...state.config,
        fileName: file ? file.name : state.config.fileName,
      },
    })),
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
}));
