import { useSceneStore } from "../stores/scene-store";

export default function ConfigPanel({
  isEditable = true,
}: {
  isEditable?: boolean;
}) {
  const voxelSize = useSceneStore((state) => state.config.voxelSize);
  const setVoxelSize = useSceneStore((state) => state.setVoxelSize);
  const showGrid = useSceneStore((state) => state.showGrid);
  const setShowGrid = useSceneStore((state) => state.setShowGrid);
  const showTexture = useSceneStore((state) => state.showTexture);
  const setShowTexture = useSceneStore((state) => state.setShowTexture);

  return (
    <div className="bg-bg-card p-4 rounded-lg border border-border-primary w-80">
      <h3 className="text-text-primary font-bold mb-4">Settings</h3>

      {/* Voxel Size Slider */}
      {isEditable && (
        <div className="mb-4">
          <label className="text-text-secondary text-xs block mb-1">
            Voxel Size (m)
          </label>
          <input
            type="range"
            min="0.1"
            max="2.0"
            step="0.1"
            value={voxelSize}
            onChange={(e) => setVoxelSize(parseFloat(e.target.value))}
            className="w-full"
          />
          <div className="text-right text-text-primary font-mono">
            {voxelSize}m
          </div>
        </div>
      )}

      {/* Grid Toggle */}
      <div className="flex items-center justify-between mb-4">
        <span className="text-sm">Show Voxel Grid</span>
        <input
          type="checkbox"
          checked={showGrid}
          onChange={(e) => setShowGrid(e.target.checked)}
          className="accent-button-primary scale-125"
        />
      </div>
      <div className="flex items-center justify-between mb-4">
        <span className="text-sm">Show Model Textures</span>
        <input
          type="checkbox"
          checked={showTexture}
          onChange={(e) => setShowTexture(e.target.checked)}
          className="accent-button-primary scale-125"
        />
      </div>

      {/* Grid Stats Info */}
      <GridStats />
    </div>
  );
}

function GridStats() {
  const gridDimensions = useSceneStore((state) => state.gridDimensions);
  const worldDimensions = useSceneStore((state) => state.worldDimensions);
  if (!gridDimensions || !worldDimensions) return null;

  const { nx, ny, nz } = gridDimensions;
  const { x, y, z } = worldDimensions;
  const total = nx * ny * nz;

  return (
    <div className="p-3 bg-black/20 rounded border border-white/5 text-xs font-mono text-text-secondary">
      <div className="mb-1 text-text-primary font-semibold">
        World Dimensions:
      </div>
      <div>
        {x.toLocaleString()} x {y.toLocaleString()} x {z.toLocaleString()}
      </div>
      <div className="mb-1 text-text-primary font-semibold">
        Grid Dimensions:
      </div>
      <div>
        {nx} x {ny} x {nz}
      </div>
      <div className="mt-2 text-text-primary font-semibold">Total Voxels:</div>
      <div>{total.toLocaleString()}</div>
    </div>
  );
}
