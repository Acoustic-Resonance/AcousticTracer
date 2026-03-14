export const simulationKeys = {
  all: ["simulations"] as const,
  lists: (userID?: string) =>
    userID
      ? ([...simulationKeys.all, "list", userID] as const)
      : [...simulationKeys.all, "list" as const],
  list: (filters: string) => [...simulationKeys.lists(), { filters }] as const,
  details: () => [...simulationKeys.all, "detail"] as const,
  detail: (id: string) => [...simulationKeys.details(), id] as const,
  rayResponses: () => [...simulationKeys.all, "rayResponse"] as const,
  rayResponse: (fileId: string) =>
    [...simulationKeys.rayResponses(), fileId] as const,
};

export const authKeys = {
  all: ["auth"] as const,
  user: () => [...authKeys.all, "user"] as const,
};
