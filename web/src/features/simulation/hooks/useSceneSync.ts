import { useEffect } from "react";
import type { Simulation } from "../api/simulation-repository";
import { useSceneStore } from "../stores/scene-store";

/**
 * Syncs a loaded simulation's config into the Zustand scene store
 * and resets ephemeral UI state when the route changes.
 *
 * Ray-response data lives entirely in TanStack Query (via
 * `useRayResponse`).  This hook only manages `resultFileId` — the
 * link between a simulation and its cached result JSON — so that
 * components deeper in the tree (e.g. VoxelGrid) can look it up
 * without prop-drilling.
 */
export default function useSceneSync(
  idOfFile: string | undefined,
  simulation: Simulation | undefined,
  _pendingFile?: File | null,
): void {
  const bounds = useSceneStore((state) => state.bounds);

  // When the simulation ID changes, reset ephemeral state so stale
  // data from a previously viewed simulation doesn't persist.
  useEffect(() => {
    if (idOfFile && idOfFile !== "new") {
      useSceneStore.getState().setPendingFile(null);
    }

    // Clear the previous resultFileId and frame index.
    useSceneStore.setState({ resultFileId: null, frameIndex: 0 });
  }, [idOfFile]);

  // Set resultFileId as soon as the simulation record is available.
  // This does NOT depend on bounds — the link to the result file is
  // known before the 3-D model finishes loading.
  useEffect(() => {
    if (!simulation) return;
    if (idOfFile === "new") return;

    const rid =
      simulation.status === "completed" && simulation.resultFileId
        ? simulation.resultFileId
        : null;

    useSceneStore.setState({ resultFileId: rid });
  }, [idOfFile, simulation]);

  // Sync the saved config into the store once the model's bounds are known.
  useEffect(() => {
    if (!bounds || !simulation?.config) return;
    if (idOfFile === "new") return;

    useSceneStore.setState({
      config: {
        fileName: simulation.fileName,
        voxelSize: simulation.config.voxelSize,
        numRays: simulation.config.numRays,
        fps: simulation.config.fps,
        material: simulation.config.material,
        selectedSource: simulation.config.selectedSource,
      },
    });
  }, [bounds, idOfFile, simulation]);
}
