import { useState } from "react";
import { useNavigate } from "react-router";
import { useUser } from "@/features/auth/context/user-store";
import UploadForm from "../components/upload-form";
import {
  useDeleteSimulation,
  useSimulationsList,
  useUpdateSimulation,
} from "../api/use-simulation-hooks";
import AppSidebar from "@/components/app-layout";
import {
  LayoutDashboard,
  Plus,
  MoreHorizontal,
  Trash2,
  Eye,
  Pencil,
  Loader2,
} from "lucide-react";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow,
} from "@/components/ui/table";
import { Button } from "@/components/ui/button";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuLabel,
  DropdownMenuSeparator,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";
export default function Dashboard() {
  const { current } = useUser();
  const navigate = useNavigate();
  const [isUploadOpen, setIsUploadOpen] = useState(false);

  const [deleteTarget, setDeleteTarget] = useState<{
    id: string;
    fileId: string;
    resultFileId?: string;
    name: string;
  } | null>(null);
  const [renameTarget, setRenameTarget] = useState<{
    id: string;
    name: string;
  } | null>(null);
  const [renameValue, setRenameValue] = useState("");
  const { data, isLoading, error, refetch } = useSimulationsList(
    current?.$id || "",
  );
  const deleteMutation = useDeleteSimulation();
  const updateMutation = useUpdateSimulation();
  const simulations = data?.simulations || [];
  const confirmDelete = () => {
    if (!deleteTarget) return;
    deleteMutation.mutate({
      id: deleteTarget.id,
      fileId: deleteTarget.fileId,
      resultFileId: deleteTarget.resultFileId,
    });
    setDeleteTarget(null);
  };

  const confirmRename = () => {
    if (!renameTarget || !renameValue.trim()) return;
    updateMutation.mutate({
      id: renameTarget.id,
      updates: { name: renameValue.trim() },
    });
    setRenameTarget(null);
  };

  return (
    <AppSidebar>
      <div className="flex-1 flex flex-col overflow-hidden">
        <header className="flex flex-row justify-between items-center px-8 pt-8 pb-2">
          <div>
            <h1 className="m-0 font-bold text-text-primary text-2xl">
              Simulations
            </h1>
            <p className="text-text-secondary text-sm mt-1">
              Manage and view your acoustic simulations
            </p>
          </div>
          {!isUploadOpen && (
            <Button onClick={() => setIsUploadOpen(true)} className="gap-2">
              <Plus className="h-4 w-4" />
              New Simulation
            </Button>
          )}
        </header>

        <main className="flex-1 overflow-y-auto px-8 pb-8 pt-4">
          {deleteMutation.isError && (
            <div className="text-danger text-sm p-3 mb-4 bg-danger/10 rounded-lg border border-danger/20">
              Delete failed: {deleteMutation.error?.message}
            </div>
          )}

          <div className="rounded-lg border border-border bg-bg-card">
            <Table>
              <TableHeader>
                <TableRow className="hover:bg-transparent border-border">
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider">
                    Date
                  </TableHead>
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider">
                    Name
                  </TableHead>
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider">
                    Status
                  </TableHead>
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider hidden md:table-cell">
                    Voxel Size
                  </TableHead>
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider hidden lg:table-cell">
                    Voxels
                  </TableHead>
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider hidden lg:table-cell">
                    FPS
                  </TableHead>
                  <TableHead className="text-text-secondary font-semibold uppercase text-xs tracking-wider hidden md:table-cell">
                    Rays
                  </TableHead>
                  <TableHead className="w-12"></TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {isLoading ? (
                  <TableRow className="hover:bg-transparent">
                    <TableCell
                      colSpan={8}
                      className="h-32 text-center text-text-secondary"
                    >
                      <div className="flex items-center justify-center gap-2">
                        <Loader2 className="h-5 w-5 animate-spin text-button-primary" />
                        <span>Loading simulations...</span>
                      </div>
                    </TableCell>
                  </TableRow>
                ) : error ? (
                  <TableRow className="hover:bg-transparent">
                    <TableCell
                      colSpan={8}
                      className="h-32 text-center text-danger"
                    >
                      <div className="flex flex-col items-center gap-2">
                        <span>Failed to load simulations: {error.message}</span>
                        <Button
                          variant="outline"
                          size="sm"
                          onClick={() => refetch()}
                        >
                          Retry
                        </Button>
                      </div>
                    </TableCell>
                  </TableRow>
                ) : simulations.length === 0 ? (
                  <TableRow className="hover:bg-transparent">
                    <TableCell
                      colSpan={8}
                      className="h-32 text-center text-text-secondary"
                    >
                      <div className="flex flex-col items-center gap-3">
                        <LayoutDashboard className="h-10 w-10 text-text-secondary/40" />
                        <div>
                          <p className="font-medium text-text-primary">
                            No simulations yet
                          </p>
                          <p className="text-sm">
                            Create your first simulation to get started.
                          </p>
                        </div>
                        <Button
                          size="sm"
                          onClick={() => setIsUploadOpen(true)}
                          className="gap-2 mt-1"
                        >
                          <Plus className="h-4 w-4" />
                          New Simulation
                        </Button>
                      </div>
                    </TableCell>
                  </TableRow>
                ) : (
                  simulations.map((sim) => (
                    <TableRow
                      key={sim.$id}
                      onClick={() => navigate(`/scene/${sim.$id}`)}
                      className="cursor-pointer border-border hover:bg-hover-bg/50 transition-colors"
                    >
                      <TableCell className="text-text-primary font-medium whitespace-nowrap">
                        {new Date(sim.$createdAt).toLocaleDateString(
                          undefined,
                          {
                            month: "short",
                            day: "numeric",
                            year: "numeric",
                          },
                        )}
                      </TableCell>
                      <TableCell className="text-text-secondary">
                        {sim.name}
                      </TableCell>
                      <TableCell>
                        <StatusBadge status={sim.status} />
                      </TableCell>
                      <TableCell className="text-text-secondary hidden md:table-cell">
                        {sim.config.voxelSize}m
                      </TableCell>
                      <TableCell className="text-text-secondary hidden lg:table-cell">
                        {sim.numVoxels?.toLocaleString() ?? "—"}
                      </TableCell>
                      <TableCell className="text-text-secondary hidden lg:table-cell">
                        {sim.config.fps}
                      </TableCell>
                      <TableCell className="text-text-secondary hidden md:table-cell">
                        {sim.config.numRays.toLocaleString()}
                      </TableCell>
                      <TableCell className="text-right">
                        <DropdownMenu>
                          <DropdownMenuTrigger asChild>
                            <Button
                              variant="ghost"
                              size="icon"
                              className="h-8 w-8"
                              onClick={(e) => e.stopPropagation()}
                            >
                              <span className="sr-only">Open menu</span>
                              <MoreHorizontal className="h-4 w-4" />
                            </Button>
                          </DropdownMenuTrigger>
                          <DropdownMenuContent align="end">
                            <DropdownMenuLabel>Actions</DropdownMenuLabel>
                            <DropdownMenuSeparator />
                            <DropdownMenuItem
                              onClick={(e) => {
                                e.stopPropagation();
                                navigate(`/scene/${sim.$id}`);
                              }}
                            >
                              <Eye className="mr-2 h-4 w-4" />
                              View Scene
                            </DropdownMenuItem>
                            <DropdownMenuItem
                              onClick={(e) => {
                                e.stopPropagation();
                                setRenameValue(sim.name);
                                setRenameTarget({
                                  id: sim.$id,
                                  name: sim.name,
                                });
                              }}
                            >
                              <Pencil className="mr-2 h-4 w-4" />
                              Rename
                            </DropdownMenuItem>
                            <DropdownMenuItem
                              className="text-danger focus:text-danger"
                              onClick={(e) => {
                                e.stopPropagation();
                                setDeleteTarget({
                                  id: sim.$id,
                                  fileId: sim.inputFileId,
                                  resultFileId: sim.resultFileId,
                                  name: sim.name,
                                });
                              }}
                            >
                              <Trash2 className="mr-2 h-4 w-4" />
                              Delete
                            </DropdownMenuItem>
                          </DropdownMenuContent>
                        </DropdownMenu>
                      </TableCell>
                    </TableRow>
                  ))
                )}
              </TableBody>
            </Table>
          </div>

          {simulations.length > 0 && (
            <div className="flex items-center justify-end pt-3">
              <p className="text-xs text-text-secondary">
                {simulations.length} simulation{simulations.length !== 1 && "s"}
              </p>
            </div>
          )}

          {isUploadOpen && (
            <UploadForm
              onClose={() => {
                setIsUploadOpen(false);
              }}
            />
          )}
          {deleteTarget && (
            <div className="fixed inset-0 flex items-center justify-center backdrop-blur-sm z-50 bg-black/60">
              <div className="bg-bg-card rounded-xl p-6 shadow-lg border border-border-primary w-96">
                <h3 className="text-lg font-semibold text-text-primary mb-2">
                  Delete Simulation
                </h3>
                <p className="text-sm text-text-secondary mb-6">
                  Are you sure you want to delete{" "}
                  <span className="font-medium text-text-primary">
                    {deleteTarget.name}
                  </span>
                  ? This action cannot be undone.
                </p>
                <div className="flex justify-end gap-3">
                  <Button
                    variant="outline"
                    onClick={() => setDeleteTarget(null)}
                  >
                    Cancel
                  </Button>
                  <Button variant="destructive" onClick={confirmDelete}>
                    Delete
                  </Button>
                </div>
              </div>
            </div>
          )}
          {renameTarget && (
            <div className="fixed inset-0 flex items-center justify-center backdrop-blur-sm z-50 bg-black/60">
              <div className="bg-bg-card rounded-xl p-6 shadow-lg border border-border-primary w-96">
                <h3 className="text-lg font-semibold text-text-primary mb-2">
                  Rename Simulation
                </h3>
                <input
                  autoFocus
                  type="text"
                  value={renameValue}
                  onChange={(e) => setRenameValue(e.target.value)}
                  onKeyDown={(e) => {
                    if (e.key === "Enter") confirmRename();
                  }}
                  className="w-full p-2 rounded-lg bg-bg-primary text-text-primary border border-white/10 text-sm focus:outline-none focus:ring-1 focus:ring-button-primary mb-6"
                />
                <div className="flex justify-end gap-3">
                  <Button
                    variant="outline"
                    onClick={() => setRenameTarget(null)}
                  >
                    Cancel
                  </Button>
                  <Button
                    onClick={confirmRename}
                    disabled={!renameValue.trim()}
                  >
                    Rename
                  </Button>
                </div>
              </div>
            </div>
          )}
        </main>
      </div>
    </AppSidebar>
  );
}

const statusConfig = {
  completed: {
    label: "Completed",
    className: "bg-success/15 text-success border-success/25",
  },
  failed: {
    label: "Failed",
    className: "bg-danger/15 text-danger border-danger/25",
  },
  pending: {
    label: "Pending",
    className: "bg-accent/15 text-accent border-accent/25",
  },
  staging: {
    label: "Staging",
    className: "bg-button-primary/15 text-link border-button-primary/25",
  },
} as const;

function StatusBadge({ status }: { status: keyof typeof statusConfig }) {
  const config = statusConfig[status] ?? statusConfig.pending;
  return (
    <span
      className={`inline-flex items-center rounded-full border px-2.5 py-0.5 text-xs font-semibold ${config.className}`}
    >
      {config.label}
    </span>
  );
}
