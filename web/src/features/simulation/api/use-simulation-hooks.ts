// TanStack Query hooks for simulation CRUD

import { useQuery, useMutation, useQueryClient } from "@tanstack/react-query";
import { simulationRepo } from "./simulation-repository";
import type {
  CreateSimulationParams,
  UpdateSimulationParams,
  SimulationList,
} from "./simulation-repository";
import { simulationKeys } from "@/lib/query-keys";
import { parseResultBuffer } from "./parse-result-binary";

/** List all simulations for a user. */
export function useSimulationsList(userId: string) {
  return useQuery({
    queryKey: simulationKeys.lists(userId),
    queryFn: () => simulationRepo.list(userId),
    enabled: !!userId, // Only run if we have a userId
  });
}

/** Get a single simulation by ID. */
export function useSimulationDetail(id: string | undefined) {
  return useQuery({
    queryKey: simulationKeys.detail(id || ""),
    queryFn: () => simulationRepo.getById(id!),
    enabled: !!id && id !== "new",
  });
}

/** Create a new simulation and invalidate the list cache. */
export function useCreateSimulation() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: (params: CreateSimulationParams) =>
      simulationRepo.create(params),
    onSuccess: () => {
      // Invalidate list to show the new simulation
      queryClient.invalidateQueries({ queryKey: simulationKeys.all });
    },
  });
}

/** Update a simulation and invalidate related caches. */
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
      queryClient.invalidateQueries({ queryKey: simulationKeys.all });
      queryClient.invalidateQueries({
        queryKey: simulationKeys.detail(variables.id),
      });
    },
  });
}

/** Delete a simulation with optimistic cache removal and rollback on failure. */
export function useDeleteSimulation() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: ({
      id,
      fileId,
      resultFileId,
    }: {
      id: string;
      fileId?: string;
      resultFileId?: string;
    }) => {
      const fileIds = [fileId, resultFileId].filter(
        (fid): fid is string => !!fid,
      );
      if (fileIds.length > 0)
        return simulationRepo.deleteWithFiles(id, fileIds);
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
      queryClient.invalidateQueries({ queryKey: simulationKeys.all });
    },
  });
}

/** Upload a simulation file. */
export function useUploadSimulationFile() {
  return useMutation({
    mutationFn: (file: File) => simulationRepo.uploadFile(file),
  });
}

/** Fetch and cache a simulation's ray-response binary. Cached with staleTime: Infinity. */
export function useRayResponse(resultFileId: string | undefined) {
  return useQuery({
    queryKey: simulationKeys.rayResponse(resultFileId || ""),
    queryFn: async () => {
      const url = simulationRepo.getFileUrl(resultFileId!);
      const res = await fetch(url);
      if (!res.ok) throw new Error("Failed to fetch ray response");
      const buffer = await res.arrayBuffer();
      return parseResultBuffer(buffer);
    },
    enabled: !!resultFileId,
    staleTime: Infinity, // Ray results never change once computed
    gcTime: 10 * 60 * 1000, // Keep in cache 10 min after last use
  });
}
