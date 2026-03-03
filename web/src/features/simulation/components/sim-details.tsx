import type { SimDetails } from "../api/simulation-repository";

export default function SimDetails({ simDetails }: { simDetails: SimDetails }) {
  if (!simDetails) return null;

  return (
    <div className="ml-auto relative group">
      {/* Svg Icon */}
      <div className="text-text-secondary hover:text-text-primary cursor-help p-2 rounded-full hover:bg-white/10 transition-all duration-200">
        <svg
          xmlns="http://www.w3.org/2000/svg"
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          strokeWidth="2"
          strokeLinecap="round"
          strokeLinejoin="round"
          className="w-6 h-6"
        >
          <circle cx="12" cy="12" r="10"></circle>
          <line x1="12" y1="16" x2="12" y2="12"></line>
          <line x1="12" y1="8" x2="12.01" y2="8"></line>
        </svg>
      </div>

      {/* Hover Content */}
      <div className="absolute right-0 top-full mt-2 max-h-0 overflow-hidden opacity-0 group-hover:max-h-96 group-hover:opacity-100 transition-all duration-300 ease-in-out z-50">
        <div className="bg-bg-primary/95 backdrop-blur border border-border-primary rounded-lg shadow-xl p-4 min-w-60">
          <h3 className="text-sm font-bold text-text-primary mb-3">
            Simulation Details
          </h3>
          <div className="text-xs text-text-secondary flex flex-col gap-2.5">
            <div className="flex justify-between">
              <span>Status:</span>
              <span
                className={`${
                  simDetails.status === "completed"
                    ? "text-success"
                    : "text-accent"
                } font-bold uppercase`}
              >
                {simDetails.status}
              </span>
            </div>
            <div className="flex justify-between">
              <span>Name:</span>
              <span className="text-text-primary">{simDetails.name}</span>
            </div>
            {simDetails.status !== "staging" && (
              <>
                <div className="flex justify-between">
                  <span>Rays:</span>
                  <span className="text-text-primary">
                    {simDetails.config.numRays.toLocaleString()}
                  </span>
                </div>
                <div className="flex justify-between">
                  <span>Voxel Size:</span>
                  <span className="text-text-primary">
                    {simDetails.config.voxelSize}
                  </span>
                </div>
                <div className="flex justify-between">
                  <span>FPS:</span>
                  <span className="text-text-primary">
                    {simDetails.config.fps}
                  </span>
                </div>
                <div className="flex justify-between mt-1 pt-2 border-t border-white/10">
                  <span>Created:</span>
                  <span className="text-text-primary">
                    {new Date(simDetails.$createdAt).toLocaleDateString()}
                  </span>
                </div>
              </>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
