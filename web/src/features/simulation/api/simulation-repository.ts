import { ID, Query, type Models } from "appwrite";
import { tablesDB, storage } from "@/lib/appwrite";
import type { SimulationDocument } from "@/api/contracts";

const CONFIG = {
  databaseId: import.meta.env.VITE_APPWRITE_DATABASE_ID,
  tableId: import.meta.env.VITE_APPWRITE_TABLE_ID_SIMULATIONS,
  bucketId: import.meta.env.VITE_APPWRITE_BUCKET_ID_SIMULATIONS,
} as const;

export interface Simulation {
  $id: string;
  $createdAt: string;
  $updatedAt: string;
  name: string;
  status: "pending" | "completed" | "failed" | "staging";
  userId: string;
  inputFileId: string;
  resultFileId?: string;
  computeTimeMs?: number;
  numVoxels?: number;
  fileName: string;
  config: {
    voxelSize: number;
    fps: number;
    numRays: number;
    material: string;
    selectedSource: {
      position: {
        x: number;
        y: number;
        z: number;
      };
      direction: {
        x: number;
        y: number;
        z: number;
      };
    };
  };
}
export interface StagingSimDetails {
  status: "staging";
  name: string;
  inputFileId: null;
}

export type SimDetails = Simulation | StagingSimDetails;

export interface SimulationList {
  simulations: Simulation[];
  total: number;
}

export interface CreateSimulationParams {
  userId: string;
  name: string;
  fileId: string;
  fileName: string;
  numVoxels: number;
  config: {
    voxelSize: number;
    fps: number;
    numRays: number;
    material: string;
    selectedSource: {
      position: {
        x: number;
        y: number;
        z: number;
      };
      direction: {
        x: number;
        y: number;
        z: number;
      };
    };
  };
}

export interface UpdateSimulationParams {
  name?: string;
  status?: Simulation["status"];
  resultFileId?: string;
  computeTimeMs?: number;
  numVoxels?: number;
}

// Auto-convert snake_case to camelCase

function documentToSimulation(doc: SimulationDocument): Simulation {
  return {
    $id: doc.$id,
    $createdAt: doc.$createdAt,
    $updatedAt: doc.$updatedAt,
    name: doc.name,
    status: doc.status,
    userId: doc.user_id,
    inputFileId: doc.input_file_id,
    resultFileId: doc.result_file_id,
    computeTimeMs: doc.compute_time_ms,
    numVoxels: doc.num_voxels,
    fileName: doc.file_name,
    config: {
      voxelSize: doc.voxel_size,
      fps: doc.fps,
      numRays: doc.num_rays,
      material: doc.material,
      selectedSource: {
        position: {
          x: doc.position_x,
          y: doc.position_y,
          z: doc.position_z,
        },
        direction: {
          x: doc.direction_x,
          y: doc.direction_y,
          z: doc.direction_z,
        },
      },
    },
  };
}

//  Auto-convert camelCase to snake_case
function simulationToRowData(params: CreateSimulationParams) {
  return {
    name: params.name,
    status: "pending" as const,
    user_id: params.userId,
    input_file_id: params.fileId,
    file_name: params.fileName,
    voxel_size: params.config.voxelSize,
    fps: params.config.fps,
    num_rays: params.config.numRays,
    material: params.config.material,
    position_x: params.config.selectedSource.position.x,
    position_y: params.config.selectedSource.position.y,
    position_z: params.config.selectedSource.position.z,
    direction_x: params.config.selectedSource.direction.x,
    direction_y: params.config.selectedSource.direction.y,
    direction_z: params.config.selectedSource.direction.z,
    num_voxels: params.numVoxels,
  };
}

// The repository class — encapsulates all simulation CRUD

class SimulationRepository {
  /** List simulations for a user, newest first. */
  async list(userId: string): Promise<SimulationList> {
    const response = await tablesDB.listRows({
      databaseId: CONFIG.databaseId,
      tableId: CONFIG.tableId,
      queries: [Query.equal("user_id", userId), Query.orderDesc("$createdAt")],
    });

    return {
      simulations: response.rows.map((row) =>
        documentToSimulation(row as unknown as SimulationDocument),
      ),
      total: response.total,
    };
  }

  /** Get a single simulation by ID. */
  async getById(id: string): Promise<Simulation> {
    const doc = await tablesDB.getRow({
      databaseId: CONFIG.databaseId,
      tableId: CONFIG.tableId,
      rowId: id,
    });

    return documentToSimulation(doc as unknown as SimulationDocument);
  }

  /** Create a new simulation. */
  async create(params: CreateSimulationParams): Promise<Simulation> {
    const doc = await tablesDB.createRow({
      databaseId: CONFIG.databaseId,
      tableId: CONFIG.tableId,
      rowId: ID.unique(),
      data: simulationToRowData(params),
    });

    return documentToSimulation(doc as unknown as SimulationDocument);
  }

  /** Partially update a simulation. */
  async update(
    id: string,
    updates: UpdateSimulationParams,
  ): Promise<Simulation> {
    const data: Record<string, unknown> = {};

    // Only include fields that were provided
    if (updates.name !== undefined) data.name = updates.name;
    if (updates.status !== undefined) data.status = updates.status;
    if (updates.resultFileId !== undefined)
      data.result_file_id = updates.resultFileId;
    if (updates.computeTimeMs !== undefined)
      data.compute_time_ms = updates.computeTimeMs;
    if (updates.numVoxels !== undefined) data.num_voxels = updates.numVoxels;

    const doc = await tablesDB.updateRow({
      databaseId: CONFIG.databaseId,
      tableId: CONFIG.tableId,
      rowId: id,
      data,
    });

    return documentToSimulation(doc as unknown as SimulationDocument);
  }

  /** Delete a simulation by ID. */
  async delete(id: string): Promise<void> {
    await tablesDB.deleteRow({
      databaseId: CONFIG.databaseId,
      tableId: CONFIG.tableId,
      rowId: id,
    });
  }

  // File Operations

  /** Upload a simulation file. */
  async uploadFile(file: File): Promise<Models.File> {
    return await storage.createFile({
      bucketId: CONFIG.bucketId,
      fileId: ID.unique(),
      file: file,
    });
  }

  /** Get a download URL for a stored file. */
  getFileUrl(fileId: string): string {
    // Use getFileDownload for better browser caching
    return storage.getFileDownload({
      bucketId: CONFIG.bucketId,
      fileId: fileId,
    });
  }

  /** Delete a file from storage. */
  async deleteFile(fileId: string): Promise<void> {
    await storage.deleteFile({
      bucketId: CONFIG.bucketId,
      fileId: fileId,
    });
  }

  /** Delete a simulation and its associated file (file failure is non-fatal). */
  async deleteWithFile(id: string, fileId: string): Promise<void> {
    // Delete simulation first (more important)
    await this.delete(id);

    try {
      await this.deleteFile(fileId);
    } catch (error) {
      console.warn(`Failed to delete file ${fileId}:`, error);
    }
  }

  /** Delete a simulation and all associated files (input + result). */
  async deleteWithFiles(id: string, fileIds: string[]): Promise<void> {
    await this.delete(id);

    await Promise.allSettled(
      fileIds.map((fid) =>
        this.deleteFile(fid).catch((error) =>
          console.warn(`Failed to delete file ${fid}:`, error),
        ),
      ),
    );
  }
}

export const simulationRepo = new SimulationRepository();
