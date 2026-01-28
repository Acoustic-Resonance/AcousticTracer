// TODO: Implement backend client functions.
// Expected backend (MVP):
// - POST   /api/simulations               -> { id }
// - GET    /api/simulations/:id/status    -> { status, progress?, error? }
// - GET    /api/simulations/:id/meta      -> meta json
// - GET    /api/simulations/:id/chunks/:i -> binary

export async function createSimulation() {
  throw new Error("TODO");
}

export async function getSimulationStatus() {
  throw new Error("TODO");
}

export async function getSimulationMeta() {
  throw new Error("TODO");
}
