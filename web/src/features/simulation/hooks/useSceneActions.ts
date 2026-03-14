import { useState } from "react";
import { useNavigate } from "react-router";
import {
  useCreateSimulation,
  useUploadSimulationFile,
  runRaytracer,
  simulationRepo,
} from "@/api/simulations";
import { useSceneStore } from "../stores/scene-store";
import { useUser } from "@/features/auth/context/user-store";
import { account } from "@/lib/appwrite";
import type { SimDetails } from "../api/simulation-repository";
import { useErrorBoundary } from "react-error-boundary";
import { queryClient } from "@/lib/query-client";
import { simulationKeys } from "@/lib/query-keys";
import { parseResultBuffer } from "../api/parse-result-binary";

export interface SceneActionsResult {
  handleStartSimulation: () => Promise<void>;
  isSubmitting: boolean;
  startError: string | null;
  clearStartError: () => void;
}

export default function useSceneActions(
  simDetails: SimDetails | undefined,
): SceneActionsResult {
  const navigate = useNavigate();
  const { current } = useUser();
  const uploadMutation = useUploadSimulationFile();
  const createMutation = useCreateSimulation();
  const { showBoundary } = useErrorBoundary();
  const [startError, setStartError] = useState<string | null>(null);

  const handleStartSimulation = async () => {
    const { bounds, config, pendingFile, voxelCount } =
      useSceneStore.getState();

    if (!bounds || !current?.$id) return;

    setStartError(null);

    let fileId = simDetails?.inputFileId;
    let simulationId: string | undefined;
    try {
      if (!fileId && pendingFile) {
        const uploadedFile = await uploadMutation.mutateAsync(pendingFile);
        fileId = uploadedFile.$id;
      }
      if (!fileId) throw new Error("No file ID available");

      const createdSimulation = await createMutation.mutateAsync({
        userId: current.$id,
        name: simDetails?.name || "Untitled",
        fileName: pendingFile?.name || "test",
        fileId,
        numVoxels: voxelCount ?? 0,
        config,
      });
      simulationId = createdSimulation.$id;
    } catch (err: unknown) {
      showBoundary(err);
      return;
    }

    try {
      // Navigate immediately — raytracer work continues in background
      navigate("/dashboard");

      runRaytracer(config)
        .then(async (raytracerResponse) => {
          // Guard: if the user logged out while the raytracer was running, bail
          const activeUser = await account.get().catch(() => null);
          if (!activeUser) return;

          let resultFileId: string | undefined;
          try {
            const resultFile = new File(
              [raytracerResponse],
              `result-${simulationId}.atrb`,
              { type: "application/octet-stream" },
            );
            const uploaded = await simulationRepo.uploadFile(resultFile);
            resultFileId = uploaded.$id;
          } catch {
            console.warn(
              "Failed to upload result file, status will still update",
            );
          }

          // Pre-populate the ray response cache so the playback slider
          // is available instantly when the user opens the scene.
          if (resultFileId) {
            const parsed = parseResultBuffer(raytracerResponse);
            queryClient.setQueryData(
              simulationKeys.rayResponse(resultFileId),
              parsed,
            );
          }

          if (simulationId) {
            await simulationRepo.update(simulationId, {
              status: "completed",
              resultFileId,
            });
            queryClient.invalidateQueries({ queryKey: simulationKeys.lists() });
          }
        })
        .catch(async (err: unknown) => {
          // Guard: skip DB updates if user logged out
          const activeUser = await account.get().catch(() => null);
          if (!activeUser) return;

          if (simulationId) {
            try {
              await simulationRepo.update(simulationId, {
                status: "failed",
              });
              queryClient.invalidateQueries({
                queryKey: simulationKeys.lists(),
              });
            } catch {
              // Swallow status-update errors to surface the original failure
            }
          }
          console.error(
            "Raytracer failed:",
            err instanceof Error ? err.message : err,
          );
        });
    } catch (err: unknown) {
      const message =
        err instanceof Error ? err.message : "Raytracer failed. Please retry.";
      setStartError(message);
    }
  };

  const isSubmitting = uploadMutation.isPending || createMutation.isPending;

  return {
    handleStartSimulation,
    isSubmitting,
    startError,
    clearStartError: () => setStartError(null),
  };
}
