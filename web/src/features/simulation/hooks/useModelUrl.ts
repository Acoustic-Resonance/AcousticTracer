import type { Simulation } from "../api/simulation-repository";
import { useEffect, useState, useMemo } from "react";
import { simulationRepo } from "../api/simulation-repository";

export default function useModelUrl(
  idOfFile: string | undefined,
  simulation: Simulation | undefined,
  pendingFile: File | null,
): string | undefined
{
  const [blobUrl, setBlobUrl] = useState<string | null>(null);
  useEffect(() => {
    if (!pendingFile) {
      setBlobUrl(null);
      return;
    }
    const url = URL.createObjectURL(pendingFile);
    setBlobUrl(url);
    return () => URL.revokeObjectURL(url);
  }, [pendingFile]);

  const modelUrl = useMemo(() => {
    if (pendingFile && blobUrl) return blobUrl;
    if (idOfFile === "new") return undefined;
    if (simulation?.inputFileId) {
      return simulationRepo.getFileUrl(simulation.inputFileId);
    }
  }, [idOfFile, blobUrl, pendingFile, simulation?.inputFileId]);
  return modelUrl
}
