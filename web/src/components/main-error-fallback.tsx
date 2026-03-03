import { type FallbackProps } from "react-error-boundary";
export const MainErrorFallback = ({
  error,
  resetErrorBoundary,
}: FallbackProps) => {
  return (
    <div className="flex h-screen w-full flex-col items-center justify-center bg-bg-primary p-4 text-text-primary">
      <div className="w-full max-w-md rounded-xl border border-red-500/20 bg-bg-card p-8 shadow-2xl backdrop-blur-sm">
        <div className="mb-6 flex justify-center">
          <div className="flex h-16 w-16 items-center justify-center rounded-full bg-red-500/10">
            <svg
              xmlns="http://www.w3.org/2000/svg"
              fill="none"
              viewBox="0 0 24 24"
              strokeWidth={1.5}
              stroke="currentColor"
              className="h-8 w-8 text-red-500"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                d="M12 9v3.75m-9.303 3.376c-.866 1.5.217 3.374 1.948 3.374h14.71c1.73 0 2.813-1.874 1.948-3.374L13.949 3.378c-.866-1.5-3.032-1.5-3.898 0L2.697 16.126zM12 15.75h.007v.008H12v-.008z"
              />
            </svg>
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
