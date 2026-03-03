import { useParams, useNavigate, useSearchParams } from "react-router";
import { useSimulationDetail } from "@/api/simulations";
import { ErrorBoundary } from "react-error-boundary";
import SceneCanvas from "../components/scene-viewer";
import SimDetails from "../components/sim-details";
import ConfigPanel from "../components/config-panel";
import UploadErrorFallback from "../components/upload-error-fallback";
import { useSceneStore } from "../stores/scene-store";
import useSceneSync from "../hooks/useSceneSync";
import useModelUrl from "../hooks/useModelUrl";
import useSimDetails from "../hooks/useSimDetails";
import useSceneActions from "../hooks/useSceneActions";
export default function Scene() {
  const { idOfFile } = useParams();
  const [searchParams] = useSearchParams();
  const navigate = useNavigate();
  const { data: simulation, isLoading, error } = useSimulationDetail(idOfFile);
  const simName = searchParams.get("name");

  const rayTracerData = useSceneStore((state) => state.rayResponse);
  const frameIndex = useSceneStore((state) => state.frameIndex);
  const setFrameIndex = useSceneStore((state) => state.setFrameIndex);

  const frameCount = rayTracerData ? Object.keys(rayTracerData).length : 0;

  const bounds = useSceneStore((state) => state.bounds);
  const pendingFile = useSceneStore((state) => state.pendingFile);

  // Sync loaded simulation config to store/ update voxel size
  useSceneSync(idOfFile, simulation, pendingFile);

  // Load ModelURl
  const modelUrl = useModelUrl(idOfFile, simulation, pendingFile);

  // Load simDetails
  const simDetails = useSimDetails(idOfFile, simName, simulation);

  // Handle Running of Simulation
  const { handleStartSimulation, isSubmitting, startError, clearStartError } =
    useSceneActions(simDetails);

  return (
    <div className="flex flex-col h-screen bg-bg-primary text-text-primary overflow-hidden">
      <header className="flex-none flex items-center p-4 gap-4 bg-bg-primary border-b border-white/5 relative z-20">
        <button
          className="flex items-center gap-2 px-3 py-2 rounded-lg text-text-secondary hover:text-text-primary hover:bg-white/10 transition-colors cursor-pointer border-none bg-transparent font-medium"
          onClick={() => navigate("/dashboard")}
        >
          <span>←</span> Back
        </button>
        <h1 className="text-xl font-bold text-text-primary m-0">
          {simDetails?.name || "Scene Viewer"}
          {simDetails?.status === "staging" && (
            <span className="ml-3 text-xs bg-yellow-500/20 text-yellow-500 px-2 py-1 rounded">
              Draft
            </span>
          )}
        </h1>

        <div className="ml-auto flex items-center gap-4">
          {/* Staging Actions */}
          {simDetails?.status === "staging" && (
            <button
              onClick={handleStartSimulation}
              disabled={isLoading || isSubmitting || !bounds}
              className="px-4 py-2 bg-button-primary text-white text-sm font-semibold rounded hover:bg-button-hover disabled:opacity-50 disabled:cursor-wait"
            >
              {isSubmitting ? "Starting..." : "Run Simulation"}
            </button>
          )}
          {simDetails && <SimDetails simDetails={simDetails} />}
        </div>
      </header>
      <main className="flex-1 p-4 h-full min-h-0 relative">
        <div className="flex gap-4 h-full">
          <div className="flex-1 h-full">
            <div className="w-full h-full bg-bg-card rounded-xl shadow-md overflow-hidden relative flex items-center justify-center border border-border-primary">
              {isLoading && (
                <div className="absolute inset-0 z-50 flex items-center justify-center bg-bg-card/80 backdrop-blur">
                  <div className="text-text-primary px-4 py-2 rounded shadow-lg border border-border-primary font-medium bg-bg-card">
                    Initializing Scene...
                  </div>
                </div>
              )}
              {isSubmitting && (
                <div className="absolute inset-0 z-50 flex items-center justify-center bg-black/40 backdrop-blur-sm cursor-wait">
                  <div className="text-text-primary px-4 py-2 rounded shadow-lg border border-border-primary font-medium bg-bg-card">
                    Building Simulation...
                  </div>
                </div>
              )}
              {error && (
                <div className="text-danger bg-red-500/10 px-4 py-2 rounded font-medium">
                  {error?.message}
                </div>
              )}
              {!isLoading && !error && modelUrl && (
                <ErrorBoundary FallbackComponent={UploadErrorFallback}>
                  <div className="w-full h-full relative">
                    {startError && (
                      <div className="absolute top-4 left-1/2 -translate-x-1/2 z-10 flex items-center gap-3 bg-red-500/10 border border-red-500/30 text-danger px-4 py-2 rounded-lg text-sm font-medium shadow-lg">
                        <span>{startError}</span>
                        <button
                          onClick={clearStartError}
                          className="text-text-secondary hover:text-text-primary transition-colors text-xs underline bg-transparent border-none cursor-pointer"
                        >
                          Dismiss
                        </button>
                      </div>
                    )}
                    <SceneCanvas
                      modelUrl={modelUrl}
                      isStaging={simDetails?.status === "staging"}
                    />
                  </div>
                </ErrorBoundary>
              )}
              {rayTracerData && frameCount > 0 && (
                <div className="absolute bottom-4 left-1/2 -translate-x-1/2 z-10 flex items-center gap-3 bg-bg-card border border-border-primary px-4 py-2 rounded-lg">
                  <span className="text-text-secondary text-sm">Frame</span>
                  <input
                    type="range"
                    min={0}
                    max={frameCount - 1}
                    value={frameIndex}
                    onChange={(e) => {
                      const i = Number(e.target.value);
                      setFrameIndex(i);
                    }}
                    className="w-48"
                  />
                  <span className="text-text-primary text-sm font-mono">
                    {frameIndex} / {frameCount - 1}
                  </span>
                </div>
              )}
            </div>
          </div>
          <aside className="w-60 h-full">
            <ConfigPanel />
          </aside>
        </div>
      </main>
    </div>
  );
}
