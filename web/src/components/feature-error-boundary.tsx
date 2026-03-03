import { type FallbackProps } from "react-error-boundary";
import { useNavigate } from "react-router";

export const FeatureErrorFallback = ({
  error,
  resetErrorBoundary,
}: FallbackProps) => {
  const navigate = useNavigate();

  return (
    <div className="flex h-full w-full items-center justify-center p-8">
      <div className="w-full max-w-lg rounded-lg border border-red-500/20 bg-bg-card p-6 shadow-lg">
        <div className="mb-4 flex items-center gap-3">
          <div className="flex h-10 w-10 items-center justify-center rounded-full bg-red-500/10">
            <svg
              xmlns="http://www.w3.org/2000/svg"
              fill="none"
              viewBox="0 0 24 24"
              strokeWidth={1.5}
              stroke="currentColor"
              className="h-5 w-5 text-red-500"
            >
              <path
                strokeLinecap="round"
                strokeLinejoin="round"
                d="M12 9v3.75m-9.303 3.376c-.866 1.5.217 3.374 1.948 3.374h14.71c1.73 0 2.813-1.874 1.948-3.374L13.949 3.378c-.866-1.5-3.032-1.5-3.898 0L2.697 16.126zM12 15.75h.007v.008H12v-.008z"
              />
            </svg>
          </div>
          <h2 className="text-lg font-bold text-text-primary">
            Something went wrong
          </h2>
        </div>

        <p className="mb-4 text-sm text-text-secondary">
          This feature encountered an error. You can try again or return to the
          dashboard.
        </p>

        {/* Error message */}
        <details className="mb-4 rounded bg-black/20 p-3">
          <summary className="cursor-pointer text-xs max-w-2xl font-semibold uppercase tracking-wide text-text-secondary hover:text-text-primary">
            Error Details
          </summary>
          <pre className="mt-2 whitespace-pre-wrap wrap-break-words text-xs text-red-400">
            {error instanceof Error ? error.message : String(error)}
            {error instanceof Error && error.stack && (
              <div className="mt-2 whitespace-pre-wrap wrap-break-words text-xs text-red-300/60">
                {error.stack}
              </div>
            )}
          </pre>
        </details>

        <div className="flex gap-3">
          <button
            onClick={resetErrorBoundary}
            className="flex-1 rounded-lg bg-button-primary px-4 py-2 text-sm font-semibold text-white transition-colors hover:bg-button-hover"
          >
            Try Again
          </button>
          <button
            onClick={() => {
              resetErrorBoundary();
              navigate("/dashboard");
            }}
            className="flex-1 rounded-lg border border-white/10 bg-transparent px-4 py-2 text-sm font-medium text-text-secondary transition-colors hover:bg-white/5 hover:text-text-primary"
          >
            Go to Dashboard
          </button>
        </div>
      </div>
    </div>
  );
};
