import { useState } from "react";
import { useNavigate } from "react-router";
import { useUser } from "@/features/auth/context/user-store";
import UploadForm from "../components/upload-form";
import {
  useDeleteSimulation,
  useSimulationsList,
} from "../api/use-simulation-hooks";
export default function Dashboard() {
  const { logout, current } = useUser();
  const navigate = useNavigate();
  const [isUploadOpen, setIsUploadOpen] = useState(false);
  const [isSidebarOpen, setIsSidebarOpen] = useState(true);
  const { data, isLoading, error, refetch } = useSimulationsList(
    current?.$id || "",
  );
  const deleteMutation = useDeleteSimulation();
  const simulations = data?.simulations || [];
  const handleDelete = (id: string, fileId: string) => {
    deleteMutation.mutate({ id, fileId });
  };

  return (
    <div className="flex h-screen bg-bg-primary text-text-primary overflow-hidden">
      {/* Sidebar */}
      <aside
        className={`${
          isSidebarOpen ? "w-64" : "w-20"
        } bg-bg-card transition-all duration-300 flex flex-col border-r border-gray-700 relative shrink-0`}
      >
        <button
          className="absolute -right-3 top-6 bg-button-primary text-white rounded-full w-6 h-6 flex items-center justify-center cursor-pointer border-none z-10 hover:bg-button-hover"
          onClick={() => setIsSidebarOpen(!isSidebarOpen)}
          aria-label="Toggle Sidebar"
        >
          {isSidebarOpen ? "<" : ">"}
        </button>

        <div className="p-6 flex-1">
          <div className="flex items-center gap-3 overflow-hidden whitespace-nowrap">
            <div className="w-8 h-8 rounded-full bg-accent flex items-center justify-center text-bg-primary font-bold shrink-0">
              {current?.name?.charAt(0).toUpperCase() || "?"}
            </div>
            <div
              className={`transition-opacity duration-300 ${
                isSidebarOpen ? "opacity-100" : "opacity-0 invisible"
              }`}
            >
              <h2 className="text-sm font-bold m-0 uppercase tracking-wider text-text-secondary">
                Welcome
              </h2>
              <p className="text-base font-semibold truncate max-w-36 capitalize">
                {current?.name}
              </p>
            </div>
          </div>
        </div>

        <div className="p-4 border-t border-gray-700">
          <button
            className={`flex items-center gap-3 w-full px-4 py-2.5 rounded-lg text-text-secondary hover:text-white hover:bg-white/5 transition-colors cursor-pointer border-none ${
              !isSidebarOpen && "justify-center px-0"
            }`}
            onClick={logout}
            title="Logout"
          >
            <span className="text-xl">⎋</span>
            <span
              className={`font-semibold transition-opacity duration-300 ${
                isSidebarOpen ? "opacity-100" : "opacity-0 hidden"
              }`}
            >
              Logout
            </span>
          </button>
        </div>
      </aside>

      {/* Main Content */}
      <div className="flex-1 flex flex-col overflow-hidden">
        <header className="flex flex-row justify-between items-center px-8 pt-8 pb-4">
          <h1 className="m-0 font-bold text-text-primary text-3xl">
            Acoustic Tracer
          </h1>
          {!isUploadOpen && (
            <button
              className="px-4 py-2.5 rounded-lg bg-button-primary text-white font-semibold text-sm transition-colors cursor-pointer border-none hover:bg-button-hover focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2 disabled:opacity-60 disabled:cursor-not-allowed"
              onClick={() => setIsUploadOpen(true)}
            >
              Create new simulation
            </button>
          )}
        </header>

        <main className="flex-1 overflow-y-auto px-8 pb-8">
          {deleteMutation.isError && (
            <div className="text-danger text-sm p-2">
              Delete failed: {deleteMutation.error?.message}
            </div>
          )}
          <div className="w-full">
            <table className="w-full border-separate border-spacing-y-2 text-sm text-left">
              <thead className="text-text-primary font-bold uppercase text-[11px] tracking-widest sticky top-0 bg-bg-primary z-10">
                <tr>
                  <th className="px-6 py-4">Date</th>
                  <th className="px-6 py-4">Name</th>
                  <th className="px-6 py-4">Status</th>
                  <th className="px-6 py-4">Voxel Size</th>
                  <th className="px-6 py-4">FPS</th>
                  <th className="px-6 py-4">Rays</th>
                  <th className="px-6 py-4"></th>
                </tr>
              </thead>
              <tbody>
                {isLoading ? (
                  <tr>
                    <td
                      colSpan={8}
                      className="text-center px-6 py-5 text-text-secondary"
                    >
                      <div className="flex items-center justify-center gap-2">
                        <div className="animate-spin">⏳</div>
                        <span>Loading simulations...</span>
                      </div>
                    </td>
                  </tr>
                ) : error ? (
                  <tr>
                    <td
                      colSpan={7}
                      className="text-center px-6 py-5 text-danger"
                    >
                      Failed to load simulations: {error.message}
                      <button
                        onClick={() => refetch()}
                        className="ml-2 underline hover:no-underline"
                      >
                        Retry
                      </button>
                    </td>
                  </tr>
                ) : simulations.length === 0 ? (
                  <tr>
                    <td
                      colSpan={7}
                      className="text-center px-6 py-5 text-text-secondary"
                    >
                      No simulations found. Create one to get started.
                    </td>
                  </tr>
                ) : (
                  simulations.map((sim) => (
                    <tr
                      key={sim.$id}
                      onClick={() => navigate(`/scene/${sim.$id}`)}
                      className="bg-bg-card transition-all duration-200 shadow-md hover:bg-hover-bg hover:-translate-y-0.5 hover:shadow-lg cursor-pointer group"
                    >
                      <td className="px-6 py-5 align-middle leading-snug rounded-l-lg text-text-primary font-medium">
                        {new Date(sim.$createdAt).toLocaleString()}
                      </td>
                      <td className="px-6 py-5 align-middle text-text-secondary leading-snug">
                        {sim.name}
                      </td>
                      <td className="px-6 py-5 align-middle text-text-secondary leading-snug">
                        <span
                          className={`inline-block px-2 py-1 rounded font-bold text-[11px] uppercase tracking-wide ${
                            sim.status === "completed"
                              ? "text-success"
                              : sim.status === "failed"
                                ? "text-danger"
                                : "text-accent"
                          }`}
                        >
                          {sim.status}
                        </span>
                      </td>
                      <td className="px-6 py-5 align-middle text-text-secondary leading-snug">
                        {sim.config.voxelSize} m
                      </td>
                      <td className="px-6 py-5 align-middle text-text-secondary leading-snug">
                        {sim.config.fps}
                      </td>
                      <td className="px-6 py-5 align-middle text-text-secondary leading-snug">
                        {sim.config.numRays.toLocaleString()}
                      </td>
                      <td className="px-6 py-5 align-middle text-text-secondary leading-snug rounded-r-lg text-right">
                        <button
                          className="bg-transparent border-none text-text-secondary text-2xl cursor-pointer p-1 leading-none rounded hover:text-danger hover:bg-white/10 transition-colors focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2"
                          aria-label="Delete"
                          onClick={(e) => {
                            e.stopPropagation();
                            handleDelete(sim.$id, sim.inputFileId);
                          }}
                        >
                          &times;
                        </button>
                      </td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
          {isUploadOpen && (
            <UploadForm
              onClose={() => {
                setIsUploadOpen(false);
              }}
            />
          )}
        </main>
      </div>
    </div>
  );
}
