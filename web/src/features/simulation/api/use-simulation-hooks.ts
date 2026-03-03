// TanStack Query Hooks for Simulations with doc strings to help me remember use cases ( and for alex )

import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import { simulationRepo } from "./simulation-repository";
import type {
  CreateSimulationParams,
  UpdateSimulationParams,
  SimulationList,
} from "./simulation-repository";
import { simulationKeys } from "@/lib/query-keys";

/**
 * Hook: List all simulations for the current user
 *
 * Usage:
 * ```tsx
 * const { data, isLoading, error } = useSimulationsList(userId);
 * ```
 */
export function useSimulationsList(userId: string) {
  return useQuery({
    queryKey: simulationKeys.lists(),
    queryFn: () => simulationRepo.list(userId),
    enabled: !!userId, // Only run if we have a userId
  });
}

/**
 * Hook: Get a single simulation by ID
 *
 * Usage:
 * ```tsx
 * const { data, isLoading, error } = useSimulationDetail(id);
 * ```
 */
export function useSimulationDetail(id: string | undefined) {
  return useQuery({
    queryKey: simulationKeys.detail(id || ""),
    queryFn: () => simulationRepo.getById(id!),
    enabled: !!id && id !== "new",
  });
}

/**
 * Hook: Create a new simulation
 *
 * Usage:
 * ```tsx
 * const createMutation = useCreateSimulation();
 * createMutation.mutate({
 *   userId: '123',
 *   name: 'My Simulation',
 *   fileId: 'file-id',
 *   config: {...},
 *   dimensions: {...}
 * });
 * ```
 */
export function useCreateSimulation() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: (params: CreateSimulationParams) =>
      simulationRepo.create(params),
    onSuccess: () => {
      // Invalidate list to show the new simulation
      queryClient.invalidateQueries({ queryKey: simulationKeys.lists() });
    },
  });
}

/**
 * Hook: Update a simulation
 *
 * Usage:
 * ```tsx
 * const updateMutation = useUpdateSimulation();
 * updateMutation.mutate({
 *   id: 'sim-123',
 *   updates: { status: 'completed' }
 * });
 * ```
 */
export function useUpdateSimulation() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({
      id,
      updates,
    }: {
      id: string;
      updates: UpdateSimulationParams;
    }) => simulationRepo.update(id, updates),
    onSuccess: (_, variables) => {
      // Invalidate both the list and the specific simulation
      queryClient.invalidateQueries({ queryKey: simulationKeys.lists() });
      queryClient.invalidateQueries({
        queryKey: simulationKeys.detail(variables.id),
      });
    },
  });
}

/**
 * Hook: Delete a simulation (with optional file deletion)
 *
 * Usage:
 * ```tsx
 * const deleteMutation = useDeleteSimulation();
 * deleteMutation.mutate({ id: 'sim-123', fileId: 'file-abc' });
 * ```
 */
export function useDeleteSimulation() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({ id, fileId }: { id: string; fileId?: string }) => {
      if (fileId) return simulationRepo.deleteWithFile(id, fileId);
      return simulationRepo.delete(id);
    },

    // Remove the row from cache immediately
    onMutate: async ({ id }) => {
      await queryClient.cancelQueries({ queryKey: simulationKeys.lists() });

      // Snapshot current cache for rollback
      const previousData = queryClient.getQueryData(simulationKeys.lists());

      // Remove the simulation from the cached list
      queryClient.setQueryData(
        simulationKeys.lists(),
        (old: SimulationList | undefined) => {
          if (!old) return old;
          return {
            ...old,
            simulations: old.simulations.filter((sim) => sim.$id !== id),
            total: old.total - 1,
          };
        },
      );

      return { previousData };
    },

    // If delete fails, roll back to the snapshot
    onError: (_err, _variables, context) => {
      if (context?.previousData) {
        queryClient.setQueryData(simulationKeys.lists(), context.previousData);
      }
    },

    // After success or failure, refetch to ensure consistency
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: simulationKeys.lists() });
    },
  });
}

/**
 * Hook: Upload a simulation file
 *
 * Usage:
 * ```tsx
 * const uploadMutation = useUploadSimulationFile();
 * uploadMutation.mutate(file);
 * ```
 */
export function useUploadSimulationFile() {
  return useMutation({
    mutationFn: (file: File) => simulationRepo.uploadFile(file),
  });
}
