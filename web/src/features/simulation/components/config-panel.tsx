import { useSceneStore } from "../stores/scene-store";
import { useMemo, useEffect, useState, useRef, useCallback } from "react";

interface ConfigPanelProps {
  mode?: "staging" | "completed";
}

export default function ConfigPanel({ mode = "staging" }: ConfigPanelProps) {
  const voxelSize = useSceneStore((state) => state.config.voxelSize);
  const setVoxelSize = useSceneStore((state) => state.setVoxelSize);
  const showGrid = useSceneStore((state) => state.showGrid);
  const setShowGrid = useSceneStore((state) => state.setShowGrid);
  const showTexture = useSceneStore((state) => state.showTexture);
  const setShowTexture = useSceneStore((state) => state.setShowTexture);
  const wireframe = useSceneStore((state) => state.wireframe);
  const setWireframe = useSceneStore((state) => state.setWireframe);

  // extra controls available in scene-store
  const material = useSceneStore((state) => state.config.material);
  const setMaterial = useSceneStore((state) => state.setMaterial);
  const setPendingFile = useSceneStore((state) => state.setPendingFile);
  const selectedSource = useSceneStore((state) => state.config.selectedSource);
  const bounds = useSceneStore((state) => state.bounds);
  const worldDimensions = useSceneStore((state) => state.worldDimensions);
  const numRays = useSceneStore((state) => state.config.numRays);
  const setNumRays = useSceneStore((state) => state.setNumRays);
  const fps = useSceneStore((state) => state.config.fps);
  const setFps = useSceneStore((state) => state.setFps);
  const setVoxelCount = useSceneStore((state) => state.setVoxelCount);

  // Local string state so the user can fully clear the input while typing
  const [raysInput, setRaysInput] = useState(String(numRays));
  useEffect(() => setRaysInput(String(numRays)), [numRays]);
  const commitRays = useCallback(() => {
    const parsed = parseInt(raysInput);
    const clamped = Number.isNaN(parsed)
      ? 1
      : Math.max(1, Math.min(100000, parsed));
    setNumRays(clamped);
    setRaysInput(String(clamped));
  }, [raysInput, setNumRays]);

  // Dynamic voxel size range based on world dimensions
  const voxelRange = useMemo(() => {
    if (!worldDimensions) return { min: 0.1, max: 2.0, step: 0.1 };
    const maxDim = Math.max(
      worldDimensions.x,
      worldDimensions.y,
      worldDimensions.z,
    );
    const min = Math.max(0.01, +(maxDim / 100).toFixed(2));
    const max = +(maxDim / 3).toFixed(1);
    const step = +((max - min) / 100).toFixed(4);
    return { min, max, step };
  }, [worldDimensions]);

  // Precompute discrete voxel sizes where the grid dimensions actually change
  const discreteSteps = useMemo(() => {
    if (!worldDimensions) return [];
    const { min, max, step } = voxelRange;
    const steps: number[] = [];
    let prevKey = "";
    for (let v = min; v <= max + step * 0.5; v += step) {
      const s = Math.min(v, max);
      const nx = Math.ceil(worldDimensions.x / s);
      const ny = Math.ceil(worldDimensions.y / s);
      const nz = Math.ceil(worldDimensions.z / s);
      const key = `${nx},${ny},${nz}`;
      if (key !== prevKey) {
        steps.push(+s.toFixed(4));
        prevKey = key;
      }
    }
    return steps;
  }, [worldDimensions, voxelRange]);

  // Find the closest discrete step index for the current voxel size
  const sliderIndex = useMemo(() => {
    if (discreteSteps.length === 0) return 0;
    let closest = 0;
    let minDist = Math.abs(discreteSteps[0] - voxelSize);
    for (let i = 1; i < discreteSteps.length; i++) {
      const dist = Math.abs(discreteSteps[i] - voxelSize);
      if (dist < minDist) {
        minDist = dist;
        closest = i;
      }
    }
    return closest;
  }, [discreteSteps, voxelSize]);

  // Voxel count at current slider value
  const numVoxels = useMemo(() => {
    if (!worldDimensions) return 0;
    const nx = Math.ceil(worldDimensions.x / voxelSize);
    const ny = Math.ceil(worldDimensions.y / voxelSize);
    const nz = Math.ceil(worldDimensions.z / voxelSize);
    return nx * ny * nz;
  }, [worldDimensions, voxelSize]);

  // Keep scene store in sync
  useEffect(() => {
    setVoxelCount(numVoxels || null);
  }, [numVoxels, setVoxelCount]);

  const MAX_VOXELS = 500_000;
  const isOverLimit = numVoxels > MAX_VOXELS;

  // When a new model loads (bounds change), reset voxel size to midpoint
  // Skip for saved simulations — their voxel size is restored from Appwrite
  useEffect(() => {
    if (!bounds || mode === "completed" || discreteSteps.length === 0) return;
    const midIndex = Math.floor(discreteSteps.length / 2);
    setVoxelSize(discreteSteps[midIndex]);
  }, [bounds, mode, discreteSteps]);

  return (
    <div className="bg-bg-card rounded-xl border border-border-primary w-full min-h-0 flex-1 flex flex-col overflow-hidden">
      {/* Scrollable body */}
      <div className="flex-1 overflow-y-auto scrollbar-none p-4 space-y-4">
        <h3 className="text-text-primary font-bold text-sm tracking-wide uppercase">
          Configuration
        </h3>

        {/* ── Voxel Size ── */}
        {mode === "staging" && (
          <Section label={`Voxel Size: ${voxelSize}m`}>
            {discreteSteps.length > 0 && (
              <>
                <input
                  type="range"
                  min={0}
                  max={discreteSteps.length - 1}
                  step={1}
                  value={sliderIndex}
                  onChange={(e) =>
                    setVoxelSize(discreteSteps[parseInt(e.target.value)])
                  }
                  className="w-full accent-button-primary"
                />
                <div className="flex justify-between text-[10px] text-text-secondary mt-0.5">
                  <span>{discreteSteps[0]}m</span>
                  <span>{discreteSteps[discreteSteps.length - 1]}m</span>
                </div>
                <div
                  className={`text-[10px] mt-1 ${isOverLimit ? "text-danger font-semibold" : "text-text-secondary"}`}
                >
                  {numVoxels.toLocaleString()} voxels
                  {isOverLimit && " — May be slow"}
                </div>
              </>
            )}
          </Section>
        )}

        {/* ── Display toggles ── */}
        <div className="space-y-1.5">
          <Toggle
            label="Voxel Grid"
            checked={showGrid}
            onChange={setShowGrid}
          />
          <Toggle
            label="Texture"
            checked={showTexture}
            onChange={(e) => {
              setShowTexture(e);
              if (e) setWireframe(false);
            }}
          />
          <Toggle
            label="Wireframe"
            checked={wireframe}
            onChange={(e) => {
              setWireframe(e);
              if (e) setShowTexture(false);
            }}
          />
        </div>

        {/* ── Staging-only controls ── */}
        {mode === "staging" && (
          <>
            {/* Material */}
            <Section label="Material">
              <select
                value={material}
                onChange={(e) => setMaterial(e.target.value)}
                className="w-full p-2 rounded-lg bg-bg-primary text-text-primary border border-white/10 text-sm focus:outline-none focus:ring-1 focus:ring-button-primary"
              >
                <option>Plastic</option>
                <option>Metal</option>
                <option>Wood</option>
              </select>
            </Section>

            {/* Replace Model */}
            <Section label="Replace Model">
              <input
                type="file"
                accept=".glb"
                onChange={(e) => {
                  const file =
                    e.target.files && e.target.files[0]
                      ? e.target.files[0]
                      : null;
                  setPendingFile(file);
                }}
                className="w-full text-sm text-text-secondary file:mr-3 file:py-1.5 file:px-3 file:rounded-lg file:border-0 file:text-sm file:font-medium file:bg-button-primary file:text-white file:cursor-pointer hover:file:bg-button-hover"
              />
            </Section>

            {/* Number of Rays */}
            <Section label="Number of Rays">
              <input
                type="number"
                min="1"
                max="100000"
                step="10"
                value={raysInput}
                onChange={(e) => setRaysInput(e.target.value)}
                onBlur={commitRays}
                onKeyDown={(e) => {
                  if (e.key === "Enter") commitRays();
                }}
                className="w-full p-2 rounded-lg bg-bg-primary text-text-primary border border-white/10 text-sm focus:outline-none focus:ring-1 focus:ring-button-primary [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
              />
            </Section>

            {/* FPS */}
            <Section label={`FPS: ${fps}`}>
              <input
                type="range"
                min="1"
                max="120"
                step="1"
                value={fps}
                onChange={(e) => setFps(parseInt(e.target.value))}
                className="w-full accent-button-primary"
              />
            </Section>
          </>
        )}

        {/* ── Grid Stats ── */}
        <GridStats />

        {/* ── Source Details (collapsible, pinned to bottom) ── */}
        <SourceDetails selectedSource={selectedSource} />
      </div>
    </div>
  );
}

/* ── Helper Components ── */

function Section({
  label,
  children,
}: {
  label: string;
  children: React.ReactNode;
}) {
  return (
    <div>
      <label className="text-text-secondary text-xs font-medium block mb-1">
        {label}
      </label>
      {children}
    </div>
  );
}

function Toggle({
  label,
  checked,
  onChange,
}: {
  label: string;
  checked: boolean;
  onChange: (v: boolean) => void;
}) {
  return (
    <div className="flex items-center justify-between py-1.5 px-2 rounded-lg hover:bg-white/5 transition-colors">
      <span className="text-sm text-text-primary">{label}</span>
      <input
        type="checkbox"
        checked={checked}
        onChange={(e) => onChange(e.target.checked)}
        className="accent-button-primary scale-110 cursor-pointer"
      />
    </div>
  );
}

function ReadOnlyField({ value }: { value: number }) {
  return (
    <div className="p-2 rounded-lg bg-bg-primary text-text-secondary text-center text-sm select-none opacity-50 border border-white/5">
      {value.toFixed(2)}
    </div>
  );
}

function SourceDetails({
  selectedSource,
}: {
  selectedSource: {
    position: { x: number; y: number; z: number };
    direction: { x: number; y: number; z: number };
  };
}) {
  const [open, setOpen] = useState(false);
  const contentRef = useRef<HTMLDivElement>(null);
  const scrollRef = useRef<HTMLDivElement>(null);

  // When the section opens, scroll the panel down so the content is visible
  useEffect(() => {
    if (open && contentRef.current) {
      // Small delay so the transition has started and height is non-zero
      const timer = setTimeout(() => {
        contentRef.current?.scrollIntoView({
          behavior: "smooth",
          block: "end",
        });
      }, 50);
      return () => clearTimeout(timer);
    }
  }, [open]);

  return (
    <div className="border-t border-white/5 pt-3" ref={scrollRef}>
      <button
        type="button"
        onClick={() => setOpen((prev) => !prev)}
        className="w-full flex items-center justify-between py-2 px-2 rounded-lg text-sm font-medium text-text-secondary hover:text-text-primary hover:bg-white/5 transition-colors cursor-pointer bg-transparent border-none"
      >
        <span>Source Details</span>
        <span
          className={`text-xs transition-transform duration-200 ${open ? "rotate-180" : ""}`}
        >
          ▼
        </span>
      </button>

      <div
        className="overflow-hidden transition-[max-height] duration-300 ease-in-out"
        style={{ maxHeight: open ? "300px" : "0px" }}
      >
        <div ref={contentRef} className="space-y-3 pt-2 pb-1">
          {/* Position */}
          <div>
            <div className="text-text-secondary text-[11px] font-medium mb-1 flex items-center gap-1.5">
              <span className="inline-block w-1.5 h-1.5 rounded-full bg-accent" />
              Position
            </div>
            <div className="grid grid-cols-3 gap-1.5">
              <ReadOnlyField value={selectedSource.position.x} />
              <ReadOnlyField value={selectedSource.position.y} />
              <ReadOnlyField value={selectedSource.position.z} />
            </div>
            <div className="grid grid-cols-3 gap-1.5 mt-0.5">
              <span className="text-[9px] text-text-secondary text-center">
                X
              </span>
              <span className="text-[9px] text-text-secondary text-center">
                Y
              </span>
              <span className="text-[9px] text-text-secondary text-center">
                Z
              </span>
            </div>
          </div>

          {/* Direction */}
          <div>
            <div className="text-text-secondary text-[11px] font-medium mb-1 flex items-center gap-1.5">
              <span className="inline-block w-1.5 h-1.5 rounded-full bg-button-primary" />
              Direction
            </div>
            <div className="grid grid-cols-3 gap-1.5">
              <ReadOnlyField value={selectedSource.direction.x} />
              <ReadOnlyField value={selectedSource.direction.y} />
              <ReadOnlyField value={selectedSource.direction.z} />
            </div>
            <div className="grid grid-cols-3 gap-1.5 mt-0.5">
              <span className="text-[9px] text-text-secondary text-center">
                X
              </span>
              <span className="text-[9px] text-text-secondary text-center">
                Y
              </span>
              <span className="text-[9px] text-text-secondary text-center">
                Z
              </span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}

function GridStats() {
  const gridDimensions = useSceneStore((state) => state.gridDimensions);
  const worldDimensions = useSceneStore((state) => state.worldDimensions);
  if (!gridDimensions || !worldDimensions) return null;

  const { nx, ny, nz } = gridDimensions;
  const { x, y, z } = worldDimensions;

  return (
    <div className="p-3 bg-black/20 rounded-lg border border-white/5 text-xs font-mono text-text-secondary space-y-1">
      <div className="text-text-primary font-semibold text-[11px] uppercase tracking-wide">
        World
      </div>
      <div>
        {x.toFixed(2)} × {y.toFixed(2)} × {z.toFixed(2)}
      </div>
      <div className="text-text-primary font-semibold text-[11px] uppercase tracking-wide pt-1">
        Grid
      </div>
      <div>
        {nx} × {ny} × {nz}
      </div>
    </div>
  );
}
