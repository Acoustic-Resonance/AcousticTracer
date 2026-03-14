import { useMemo } from "react";
import type { SimDetails } from "../api/simulation-repository";

export default function useSimDetails(
  idOfFile: string | undefined,
  simName: string | null,
  simDetails: SimDetails | undefined,
): SimDetails | undefined {
  return useMemo(() => {
    if (idOfFile === "new") {
      return {
        name: simName || "New Simulation",
        status: "staging" as const,
        inputFileId: null,
      };
    }
    return simDetails;
  }, [idOfFile, simName, simDetails]);
}
