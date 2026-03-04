import { useState } from "react";
import { useNavigate } from "react-router";
import { useSceneStore } from "../stores/scene-store";
import { getErrorMessage } from "@/utils/get-error-message";

interface UploadFormProps {
  onClose?: () => void;
}

export default function UploadForm({ onClose }: UploadFormProps) {
  const [file, setFile] = useState<File | null>(null);
  const [name, setName] = useState("New Room");
  const [formError, setFormError] = useState<string | null>(null);
  const navigate = useNavigate();
  const setPendingFile = useSceneStore((state) => state.setPendingFile);

  const handleSubmit = async (e: React.SubmitEvent<HTMLFormElement>) => {
    e.preventDefault();
    if (!file) {
      setFormError("Please select a .glb file to upload");
      return;
    }
    if (!file.name.toLowerCase().endsWith(".glb")) {
      setFormError("Only .glb files are supported.");
      return;
    }

    try {
      // 1. Store the file in global state instead of uploading immediately
      setPendingFile(file);
      // 2. Close the modal
      if (onClose) onClose();

      // 3. Navigate to scene view with just the name
      // File will be retrieved from store
      navigate(`/scene/new?name=${encodeURIComponent(name)}`);
    } catch (err: unknown) {
      setFormError(getErrorMessage(err));
    }
  };

  return (
    <div className="fixed inset-0 flex items-center justify-center backdrop-blur-sm z-1000 p-4 bg-black/75">
      <div className="bg-bg-card rounded-xl p-6 shadow-md w-112.5 border border-border-primary">
        <div className="flex justify-between items-center mb-6 pb-4 border-b border-border-primary">
          <h3 className="m-0 text-lg font-semibold text-text-primary">
            New Project
          </h3>
          {onClose && (
            <button
              onClick={onClose}
              className="text-2xl font-bold p-1 leading-none rounded cursor-pointer transition-all duration-200 text-text-secondary bg-transparent hover:text-text-primary hover:bg-white/10 focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2"
              aria-label="Close"
            >
              &times;
            </button>
          )}
        </div>
        <form onSubmit={handleSubmit} className="flex flex-col gap-5">
          {/* Name Field */}
          <div>
            <label className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide">
              Project Name
            </label>
            <input
              type="text"
              value={name}
              onChange={(e) => setName(e.target.value)}
              required
              className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
            />
          </div>

          {/* File Upload */}
          <div>
            <label className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide">
              Room Model (.glb)
            </label>
            <div className="relative">
              <input
                type="file"
                id="model-upload"
                accept=".glb"
                onChange={(e) => setFile(e.target.files?.[0] ?? null)}
                className="hidden"
              />
              <label
                htmlFor="model-upload"
                className="flex items-center w-full py-2 px-2 border border-border-primary bg-input-bg rounded-lg text-sm transition-colors cursor-pointer hover:border-button-primary"
              >
                <span className="bg-button-primary text-white px-3 py-1.5 rounded-lg text-xs mr-3 font-medium hover:bg-opacity-90 transition-opacity">
                  Browse
                </span>
                <span
                  className={`truncate ${
                    !file ? "text-text-secondary" : "text-text-primary"
                  }`}
                >
                  {file ? file.name : "No file selected"}
                </span>
              </label>
            </div>
          </div>
          {formError && (
            <div className="text-danger m-1 text-sm p-2 bg-red-500/10 rounded">
              {formError}
            </div>
          )}
          <button className="w-full px-4 py-3 rounded-lg bg-button-primary text-white font-semibold text-sm cursor-pointer border-none hover:bg-button-hover focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2 disabled:opacity-60 disabled:cursor-not-allowed flex justify-center items-center">
            Continue to Editor
          </button>
        </form>
      </div>
    </div>
  );
}
