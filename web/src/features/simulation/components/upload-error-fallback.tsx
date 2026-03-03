import { useState, useRef } from "react";
import { type FallbackProps } from "react-error-boundary";
import { useSceneStore } from "../stores/scene-store";

export default function UploadErrorFallback({
  error: _error,
  resetErrorBoundary,
}: FallbackProps) {
  void _error;
  const fileRef = useRef<HTMLInputElement>(null);
  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const setPendingFile = useSceneStore((s) => s.setPendingFile);

  const handleReupload = () => {
    if (!selectedFile) return;
    setPendingFile(selectedFile);
    setSelectedFile(null);
    resetErrorBoundary();
  };

  return (
    <div className="flex h-full w-full items-center justify-center p-8">
      <div className="w-full max-w-md rounded-lg border border-red-500/20 bg-bg-card p-6 shadow-lg">
        <div className="mb-4 flex items-center gap-3">
          <div className="flex h-10 w-10 shrink-0 items-center justify-center rounded-full bg-red-500/10">
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
            File Upload Failed
          </h2>
        </div>

        {/* Error message */}
        <p className="mb-2 text-sm text-text-secondary">
          The model file could not be loaded. This may be caused by an invalid
          or corrupted file.
        </p>
        {/* <div className="mb-5 rounded bg-black/20 p-3 text-xs font-mono text-red-400 wrap-break-word">
          {error instanceof Error ? error.message : String(error)}
        </div> */}

        {/* Re-upload section */}
        <div className="mb-4">
          <label className="mb-1.5 block text-xs font-semibold uppercase tracking-wide text-text-secondary">
            Upload a new file to retry
          </label>
          <div className="relative">
            <input
              ref={fileRef}
              type="file"
              accept=".glb,.gltf"
              id="reupload-model"
              onChange={(e) =>
                setSelectedFile(e.target.files?.[0] ?? null)
              }
              className="hidden"
            />
            <label
              htmlFor="reupload-model"
              className="flex w-full cursor-pointer items-center rounded-lg border border-border-primary bg-input-bg px-2 py-2 text-sm transition-colors hover:border-button-primary"
            >
              <span className="mr-3 rounded-lg bg-button-primary px-3 py-1.5 text-xs font-medium text-white transition-opacity hover:bg-opacity-90">
                Browse
              </span>
              <span
                className={`truncate ${selectedFile ? "text-text-primary" : "text-text-secondary"}`}
              >
                {selectedFile ? selectedFile.name : "No file selected"}
              </span>
            </label>
          </div>
        </div>

        {/* Action buttons */}
        <div className="flex gap-3">
          <button
            onClick={handleReupload}
            disabled={!selectedFile}
            className="flex-1 rounded-lg bg-button-primary px-4 py-2 text-sm font-semibold text-white transition-colors hover:bg-button-hover disabled:cursor-not-allowed disabled:opacity-50"
          >
            Re-upload
          </button>
          <button
            onClick={resetErrorBoundary}
            className="flex-1 rounded-lg border border-white/10 bg-transparent px-4 py-2 text-sm font-medium text-text-secondary transition-colors hover:bg-white/5 hover:text-text-primary"
          >
            Try Again
          </button>
        </div>
      </div>
    </div>
  );
}
