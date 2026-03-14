import { type FallbackProps } from "react-error-boundary";
import { AlertTriangle } from "lucide-react";

export const MainErrorFallback = ({
  error,
  resetErrorBoundary,
}: FallbackProps) => {
  return (
    <div className="flex h-screen w-full flex-col items-center justify-center bg-bg-primary p-4 text-text-primary">
      <div className="w-full max-w-md rounded-xl border border-red-500/20 bg-bg-card p-8 shadow-2xl backdrop-blur-sm">
        <div className="mb-6 flex justify-center">
          <div className="flex h-16 w-16 items-center justify-center rounded-full bg-red-500/10">
            <AlertTriangle className="h-8 w-8 text-red-500" />
          </div>
        </div>

        <h2 className="mb-2 text-center text-xl font-bold text-text-primary">
          Application Error
        </h2>

        <p className="mb-6 text-center text-sm text-text-secondary">
          AcousticTracer encountered a critical error and needs to restart.
        </p>

        {/* Error Details (Scrollable if long) */}
        <div className="mb-6 max-h-32 overflow-y-auto rounded bg-black/20 p-3 font-mono text-xs text-red-400">
          {error instanceof Error ? error.message : String(error)}
        </div>

        <div className="flex flex-col gap-3">
          <button
            onClick={resetErrorBoundary}
            className="w-full rounded-lg bg-button-primary px-4 py-2.5 text-sm font-semibold text-white transition-colors hover:bg-button-hover focus:outline-none focus:ring-2 focus:ring-button-primary/50"
          >
            Try Again
          </button>

          <button
            onClick={() => window.location.assign(window.location.origin)}
            className="w-full rounded-lg border border-white/10 bg-transparent px-4 py-2.5 text-sm font-medium text-text-secondary transition-colors hover:bg-white/5 hover:text-text-primary"
          >
            Reload Page
          </button>
        </div>
      </div>
    </div>
  );
};
