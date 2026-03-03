import { useEffect } from "react";
import type { Simulation } from "../api/simulation-repository";
import { useSceneStore } from "../stores/scene-store";

export default function useSceneSync(
  _idOfFile: string | undefined,
  simulation: Simulation | undefined,
  _pendingFile: File | null,
): void {
  void _idOfFile;
  void _pendingFile;

  const setVoxelSize = useSceneStore((state) => state.setVoxelSize);

  // When loading a saved simulation, restore its voxel size
  useEffect(() => {
    if (simulation?.config) {
      setVoxelSize(simulation.config.voxelSize);
    }
  }, [simulation, setVoxelSize]);
}
