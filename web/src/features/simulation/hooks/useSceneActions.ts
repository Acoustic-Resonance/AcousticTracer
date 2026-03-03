import { useState } from "react";
import { useNavigate } from "react-router";
import {
  useCreateSimulation,
  useUploadSimulationFile,
  runRaytracer,
} from "@/api/simulations";
import { useSceneStore } from "../stores/scene-store";
import { useUser } from "@/features/auth/context/user-store";
import type { SimDetails } from "../api/simulation-repository";
import { useErrorBoundary } from "react-error-boundary";

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
    const { bounds, config, pendingFile } = useSceneStore.getState();

    if (!bounds || !current?.$id) return;

    setStartError(null);

    let fileId = simDetails?.inputFileId;
    try {
      if (!fileId && pendingFile) {
        const uploadedFile = await uploadMutation.mutateAsync(pendingFile);
        fileId = uploadedFile.$id;
      }
      if (!fileId) throw new Error("No file ID available");

      await createMutation.mutateAsync({
        userId: current.$id,
        name: simDetails?.name || "Untitled",
        fileName: pendingFile?.name || "test",
        fileId,
        config,
      });
    } catch (err: unknown) {
      showBoundary(err);
      return;
    }

    try {
      console.log(config);
      const raytracerResponse = await runRaytracer(config);
      // console.log(raytracerResponse);
      useSceneStore.getState().setRayResponse(raytracerResponse);
      // navigate("/dashboard");
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
