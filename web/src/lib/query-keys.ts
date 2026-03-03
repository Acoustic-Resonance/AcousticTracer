export const simulationKeys = {
  all: ["simulations"] as const,
  lists: () => [...simulationKeys.all, "list"] as const,
  list: (filters: string) => [...simulationKeys.lists(), { filters }] as const,
  details: () => [...simulationKeys.all, "detail"] as const,
  detail: (id: string) => [...simulationKeys.details(), id] as const,
};

export const authKeys = {
  all: ["auth"] as const,
  user: () => [...authKeys.all, "user"] as const,
};