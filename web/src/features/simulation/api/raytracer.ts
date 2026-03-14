import { type Simulation } from "./simulation-repository";

const RAYTRACER_URL =
  import.meta.env.VITE_RAYTRACER_URL;

export async function runRaytracer(
  config: Simulation["config"],
): Promise<ArrayBuffer> {
  const response = await fetch(`${RAYTRACER_URL}/run`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(config),
  });

  if (!response.ok) {
    throw new Error("Error Running Raytracer");
  }

  return response.arrayBuffer();
}
