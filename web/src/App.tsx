import UploadForm from "./components/UploadForm";
import "./index.css"; // Ensure styles are imported

function App() {
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

export default App;
