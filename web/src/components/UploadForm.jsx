// TODO: Upload form + params form
// Expected behavior:
// - Pick a .glb/.gltf
// - Configure params
// - POST /api/simulations (multipart)
import { useState } from "react";

export default function UploadForm() {
  const [file, setFile] = useState(null);
  return (
    <div>
      <h3>Upload Room (.glb/.gltf)</h3>
      <input
        type="file"
        accept=".glb,.gltf"
        onChange={(e) => setFile(e.target.files?.[0] ?? null)}
      />
    </div>
  );
}
