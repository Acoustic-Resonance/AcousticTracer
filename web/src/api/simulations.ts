/**
 * Central API export for simulations
 *
 * Re-exports from the new repository pattern implementation
 */

export { simulationRepo } from "@/features/simulation/api/simulation-repository";

// Export types for convenience
export type {
  Simulation,
  SimulationList,
  CreateSimulationParams,
  UpdateSimulationParams,
} from "@/features/simulation/api/simulation-repository";

// Export hooks for convenience
export {
  useSimulationsList,
  useSimulationDetail,
  useCreateSimulation,
  useUpdateSimulation,
  useDeleteSimulation,
  useUploadSimulationFile,
} from "@/features/simulation/api/use-simulation-hooks";

export { runRaytracer } from "@/features/simulation/api/raytracer";
