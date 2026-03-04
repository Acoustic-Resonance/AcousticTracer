import { type Simulation } from "./simulation-repository";

export async function runRaytracer(
  config: Simulation["config"],
): Promise<ArrayBuffer> {
  const response = await fetch("http://127.0.0.1:8080/run", {
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
