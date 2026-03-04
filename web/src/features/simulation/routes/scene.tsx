import { useParams, useNavigate, useSearchParams } from "react-router";
import { useState, useEffect, useCallback, useRef } from "react";
import { useSimulationDetail } from "@/api/simulations";
import { useRayResponse } from "../api/use-simulation-hooks";
import { ErrorBoundary, type FallbackProps } from "react-error-boundary";
import SceneCanvas from "../components/scene-viewer";
import SimDetails from "../components/sim-details";
import ConfigPanel from "../components/config-panel";
import UploadErrorFallback from "../components/upload-error-fallback";
import { FeatureErrorFallback } from "@/components/feature-error-boundary";
import { useSceneStore } from "../stores/scene-store";
import useSceneSync from "../hooks/useSceneSync";
import useModelUrl from "../hooks/useModelUrl";
import useSimDetails from "../hooks/useSimDetails";
import useSceneActions from "../hooks/useSceneActions";
import { getErrorMessage } from "@/utils/get-error-message";

function sceneFallbackRender(props: FallbackProps) {
  const msg = getErrorMessage(props.error).toLowerCase();
  const isModelError =
    msg.includes("could not load") ||
    msg.includes("unexpected token") ||
    msg.includes("failed to parse") ||
    msg.includes("invalid glb") ||
    msg.includes("gltf") ||
    msg.includes("glb") ||
    msg.includes("arraybuffer") ||
    msg.includes("model");

  if (isModelError) return <UploadErrorFallback {...props} />;
  return <FeatureErrorFallback {...props} />;
}

export default function Scene() {
  const { idOfFile } = useParams();
  const [searchParams] = useSearchParams();
  const navigate = useNavigate();
  const { data: simulation, isLoading, error } = useSimulationDetail(idOfFile);
  const simName = searchParams.get("name");

  const resultFileId = useSceneStore((state) => state.resultFileId);
  const frameIndex = useSceneStore((state) => state.frameIndex);
  const setFrameIndex = useSceneStore((state) => state.setFrameIndex);
  const fps = useSceneStore((state) => state.config.fps);

  // Ray data lives entirely in TanStack Query — keyed by resultFileId.
  // Cached with staleTime: Infinity so revisits are instant.
  const { data: rayTracerData, isLoading: isRayDataLoading } = useRayResponse(
    resultFileId ?? undefined,
  );

  const frameCount = rayTracerData ? rayTracerData.length : 0;

  // Playback state
  const [isPlaying, setIsPlaying] = useState(false);
  const intervalRef = useRef<ReturnType<typeof setInterval> | null>(null);

  const stopPlayback = useCallback(() => {
    if (intervalRef.current) {
      clearInterval(intervalRef.current);
      intervalRef.current = null;
    }
    setIsPlaying(false);
  }, []);

  const togglePlayback = useCallback(() => {
    if (isPlaying) {
      stopPlayback();
      return;
    }
    // Restart from beginning if at the end
    if (useSceneStore.getState().frameIndex >= frameCount - 1) {
      setFrameIndex(0);
    }
    setIsPlaying(true);
  }, [isPlaying, stopPlayback, frameCount, setFrameIndex]);

  // Drive the frame index forward at the simulation's FPS
  useEffect(() => {
    if (!isPlaying || frameCount <= 1) return;
    intervalRef.current = setInterval(() => {
      const current = useSceneStore.getState().frameIndex;
      if (current >= frameCount - 1) {
        stopPlayback();
      } else {
        setFrameIndex(current + 1);
      }
    }, 1000 / fps);
    return () => {
      if (intervalRef.current) clearInterval(intervalRef.current);
    };
  }, [isPlaying, frameCount, fps, setFrameIndex, stopPlayback]);

  const bounds = useSceneStore((state) => state.bounds);
  const pendingFile = useSceneStore((state) => state.pendingFile);

  // Sync loaded simulation config + resultFileId into Zustand
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
              {!isLoading && !error && !modelUrl && (
                <div className="flex flex-col items-center gap-4 text-center px-6">
                  <div className="flex h-16 w-16 items-center justify-center rounded-2xl bg-button-primary/10">
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      className="h-8 w-8 text-button-primary"
                      fill="none"
                      viewBox="0 0 24 24"
                      stroke="currentColor"
                      strokeWidth={1.5}
                    >
                      <path
                        strokeLinecap="round"
                        strokeLinejoin="round"
                        d="M12 16v-4m0 0V8m0 4h4m-4 0H8m13 0a9 9 0 11-18 0 9 9 0 0118 0z"
                      />
                    </svg>
                  </div>
                  <div>
                    <p className="text-lg font-semibold text-text-primary">
                      No model loaded
                    </p>
                    <p className="mt-1 text-sm text-text-secondary max-w-xs">
                      Upload a{" "}
                      <span className="font-medium text-text-primary">
                        .glb
                      </span>{" "}
                      file using the config panel on the right to get started.
                    </p>
                  </div>
                </div>
              )}
              {!isLoading && !error && modelUrl && (
                <ErrorBoundary FallbackComponent={sceneFallbackRender}>
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
                      awaitingResults={
                        simDetails?.status === "completed" && !rayTracerData
                      }
                    />
                  </div>
                </ErrorBoundary>
              )}
              {simDetails?.status === "completed" &&
                !rayTracerData &&
                isRayDataLoading && (
                  <div className="absolute bottom-4 left-1/2 -translate-x-1/2 z-10 flex items-center gap-3 bg-bg-card border border-border-primary px-4 py-2 rounded-lg">
                    <div className="h-4 w-4 border-2 border-text-secondary border-t-transparent rounded-full animate-spin" />
                    <span className="text-text-secondary text-sm">
                      Loading simulation results…
                    </span>
                  </div>
                )}
              {rayTracerData && frameCount > 0 && (
                <div className="absolute bottom-4 left-1/2 -translate-x-1/2 z-10 flex items-center gap-3 bg-bg-card border border-border-primary px-4 py-2 rounded-lg">
                  <button
                    onClick={togglePlayback}
                    className="flex items-center justify-center w-7 h-7 rounded-full bg-button-primary text-white hover:bg-button-hover transition-colors cursor-pointer border-none"
                    aria-label={isPlaying ? "Pause" : "Play"}
                  >
                    {isPlaying ? (
                      <svg
                        width="10"
                        height="12"
                        viewBox="0 0 10 12"
                        fill="currentColor"
                      >
                        <rect x="0" y="0" width="3" height="12" rx="0.5" />
                        <rect x="7" y="0" width="3" height="12" rx="0.5" />
                      </svg>
                    ) : (
                      <svg
                        width="10"
                        height="12"
                        viewBox="0 0 10 12"
                        fill="currentColor"
                      >
                        <path d="M0 0.5a.5.5 0 0 1 .764-.424l9 5.5a.5.5 0 0 1 0 .848l-9 5.5A.5.5 0 0 1 0 11.5z" />
                      </svg>
                    )}
                  </button>
                  <span className="text-text-secondary text-sm">Frame</span>
                  <input
                    type="range"
                    min={0}
                    max={frameCount - 1}
                    value={frameIndex}
                    onChange={(e) => {
                      const i = Number(e.target.value);
                      setFrameIndex(i);
                      if (isPlaying) stopPlayback();
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
          {(simDetails?.status === "staging" ||
            simDetails?.status === "completed") && (
            <aside className="w-60 min-h-0 h-full flex flex-col">
              <ConfigPanel
                mode={
                  simDetails.status === "completed" ? "completed" : "staging"
                }
              />
            </aside>
          )}
        </div>
      </main>
    </div>
  );
}
