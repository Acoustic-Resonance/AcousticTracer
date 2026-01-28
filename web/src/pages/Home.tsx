import { useState } from "react";
import { useUser } from "../lib/context/user";
import UploadForm from "../components/UploadForm";
import "./Home.css";

export default function Home() {
  const { logout, current } = useUser();
  const [isUploadOpen, setIsUploadOpen] = useState(true);

  return (
    <div className="home-container">
      <header className="home-header">
        <span className="home-welcome">Welcome, {current?.email}</span>
        <button className="button" onClick={logout}>
          Logout
        </button>
      </header>

      <main className="home-main">
        {!isUploadOpen && (
          <div className="upload-trigger-container">
            <button className="button" onClick={() => setIsUploadOpen(true)}>
              Create new simulation
            </button>
          </div>
        )}

        {isUploadOpen && <UploadForm onClose={() => setIsUploadOpen(false)} />}
      </main>
    </div>
  );
}
