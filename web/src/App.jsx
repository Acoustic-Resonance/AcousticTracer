// TODO: Build the app UI state machine:
// upload -> status -> playback
//
// Suggested imports (when you implement):
// - components/UploadForm
import UploadForm from "./components/UploadForm";
// - components/SimulationStatus
// - components/PlaybackView

export default function App() {
  return (
    <div className="container">
      <div className="header">
        <h1 className="h1">AcousticTracer</h1>
        <div className="muted">Skeleton mode</div>
      </div>

      <div className="card">
        <div className="muted">
          TODO: Implement upload, polling, and playback.
        </div>
      </div>
      <div className="upload">
          <UploadForm />
      </div>
    </div>
  );
}
