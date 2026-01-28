// TODO: Upload form + params form
// Expected behavior:
// - Pick a .glb/.gltf
// - Configure params
// - POST /api/simulations (multipart)
import { useState } from "react";
import { createSimulation } from "../api/simulations";
import "./UploadForm.css";

interface UploadFormProps {
  onClose?: () => void;
}

export default function UploadForm({ onClose }: UploadFormProps) {
  const [file, setFile] = useState<File | null>(null);
  const [formData, setFormData] = useState({
    voxel_size: 0.1,
    floor_material: "concrete",
    wall_material: "plaster",
    roof_material: "acoustic_tile",
    fps: 60,
    num_rays: 10000,
    num_iterations: 100,
    area_x: 10,
    area_y: 10,
    area_z: 5,
  });

  const handleChange = (e: any) => {
    const { name, value } = e.target;
    setFormData((prev) => ({ ...prev, [name]: value }));
  };

  const handleSubmit = async (e: any) => {
    e.preventDefault();
    if (!file) {
      alert("Please select a GLB/GLTF file");
      return;
    }

    try {
      // Construct the payload matching docs/communication_standards.md
      // Note: In a real app, this would likely be a FormData object with the file
      // and a JSON string for the metadata, or two separate calls.
      // For now, we mock the call.
      console.log("Submitting simulation:", { file, ...formData });
      await createSimulation({ file, ...formData });
    } catch (err) {
      console.error("Failed to create simulation", err);
      alert("Simulation creation failed (check console)");
    }
  };

  return (
    <div className="modal-overlay">
      <div className="card modal-content">
        <div className="upload-form-header">
          <h3 className="h1 upload-form-title">New Simulation</h3>
          {onClose && (
            <button
              onClick={onClose}
              className="close-button"
              aria-label="Close"
            >
              &times;
            </button>
          )}
        </div>

        <form onSubmit={handleSubmit} className="form-stack">
          {/* File Upload */}
          <div>
            <label className="label">Room Model (.glb/.gltf)</label>
            <input
              type="file"
              accept=".glb,.gltf"
              onChange={(e) => setFile(e.target.files?.[0] ?? null)}
              required
              className="input"
            />
          </div>

          {/* Simulation Config */}
          <div className="row">
            <div>
              <label className="label">Voxel Size (m)</label>
              <input
                type="number"
                step="0.01"
                name="voxel_size"
                value={formData.voxel_size}
                onChange={handleChange}
                required
                className="input"
              />
            </div>
            <div>
              <label className="label">FPS</label>
              <input
                type="number"
                name="fps"
                value={formData.fps}
                onChange={handleChange}
                required
                className="input"
              />
            </div>
          </div>

          <div>
            <label className="label">Rays</label>
            <input
              type="number"
              name="num_rays"
              value={formData.num_rays}
              onChange={handleChange}
              required
              className="input"
            />
          </div>

          <div>
            <label className="label">Genetics Iterations</label>
            <input
              type="number"
              name="num_iterations"
              value={formData.num_iterations}
              onChange={handleChange}
              required
              className="input"
            />
          </div>

          {/* Materials */}
          <div className="section-divider">
            <span className="section-title">Materials</span>
            <div className="form-stack">
              <input
                placeholder="Floor Material"
                name="floor_material"
                value={formData.floor_material}
                onChange={handleChange}
                className="input"
              />
              <input
                placeholder="Wall Material"
                name="wall_material"
                value={formData.wall_material}
                onChange={handleChange}
                className="input"
              />
              <input
                placeholder="Roof Material"
                name="roof_material"
                value={formData.roof_material}
                onChange={handleChange}
                className="input"
              />
            </div>
          </div>

          {/* Area */}
          <div className="section-divider">
            <span className="section-title">Selected Area Dimensions</span>
            <div className="form-grid-3">
              <input
                placeholder="X"
                type="number"
                name="area_x"
                value={formData.area_x}
                onChange={handleChange}
                className="input"
              />
              <input
                placeholder="Y"
                type="number"
                name="area_y"
                value={formData.area_y}
                onChange={handleChange}
                className="input"
              />
              <input
                placeholder="Z"
                type="number"
                name="area_z"
                value={formData.area_z}
                onChange={handleChange}
                className="input"
              />
            </div>
          </div>

          <button type="submit" className="button w-full mt-2">
            Start Simulation
          </button>
        </form>
      </div>
    </div>
  );
}
