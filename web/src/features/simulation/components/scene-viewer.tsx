import { Canvas } from "@react-three/fiber";
import {
  OrbitControls,
  useGLTF,
  Bounds,
  Html,
  useProgress,
  Environment,
  GizmoHelper,
  GizmoViewport,
  AdaptiveDpr,
} from "@react-three/drei";
import { Suspense, useEffect, useRef } from "react";
import * as THREE from "three";
import { useSceneStore } from "../stores/scene-store";
import VoxelGrid from "./voxel-grid";
import SourcePlacer from "./source-placer";

interface SceneCanvasProps {
  modelUrl: string | null;
  isStaging?: boolean;
  /** True while a completed simulation's ray-response JSON is still loading.
   *  Prevents mounting the heavy full-white VoxelGrid during the fetch gap. */
  awaitingResults?: boolean;
}
function Loader() {
  const { progress } = useProgress();
  return (
    <Html center>
      <div className="text-text-primary bg-bg-card/80 backdrop-blur px-4 py-2 rounded shadow-lg border border-border-primary font-medium">
        {progress.toFixed(0)}%
      </div>
    </Html>
  );
}

function Model({
  url,
  onLoad,
}: {
  url: string;
  onLoad: (box: THREE.Box3 | null) => void;
}) {
  // useGLTF automatically caches by URL
  const { scene } = useGLTF(url);

  // Reset bounds when URL changes so VoxelGrid unmounts cleanly
  useEffect(() => {
    onLoad(null);
  }, [url, onLoad]);

  const showTexture = useSceneStore((state) => state.showTexture);
  const wireframe = useSceneStore((state) => state.wireframe);
  const material = useSceneStore((state) => state.config.material);
  // store the original texture, so the user can toggle
  const originalTexture = useRef(new Map());

  // Effect 1: Geometry setup — reset scales, store originals, compute bounds
  // Only re-runs when the scene itself changes (new model loaded)
  useEffect(() => {
    if (!scene) return;

    scene.traverse((child) => {

      if (child instanceof THREE.Mesh) {
        if (!originalTexture.current.has(child.uuid)) {
          originalTexture.current.set(child.uuid, child.material);
        }
      }
    });
    scene.updateMatrixWorld(true);

    const rawBox = new THREE.Box3().setFromObject(scene);
    const size = new THREE.Vector3();
    rawBox.getSize(size);

    const box = new THREE.Box3().setFromObject(scene);
    onLoad(box);
  }, [scene, onLoad]);

  // Effect 2: Material application — runs when texture/wireframe/material toggles change
  useEffect(() => {
    if (!scene) return;

    scene.traverse((child) => {
      if (child instanceof THREE.Mesh) {
        if (!showTexture) {
          const color = new THREE.Color();
          switch (material) {
            case "Wood":
              color.set("#5c260e");
              break;
            case "Concrete":
              color.set("#474747");
              break;
            case "Plastic":
              color.set("#246982");
              break;
            case "Metal":
              color.set("#8a8a8a");
              break;
            case "Glass":
              color.set("#88c0d0");
              break;
            default:
              color.set("#246982");
              break;
          }
          child.material = new THREE.MeshStandardMaterial({
            color,
            roughness: 0.8,
            metalness: 0.1,
            wireframe,
          });
        } else if (originalTexture.current.has(child.uuid)) {
          child.material = originalTexture.current.get(child.uuid);
        }
      }
    });
  }, [scene, showTexture, material, wireframe]);

  return <primitive object={scene} />;
}

export default function SceneCanvas({
  modelUrl,
  isStaging = false,
  awaitingResults = false,
}: SceneCanvasProps) {
  const setBounds = useSceneStore((state) => state.setBounds);
  const bounds = useSceneStore((state) => state.bounds);
  const showGrid = useSceneStore((state) => state.showGrid);

  // Preload the model for faster loading
  useEffect(() => {
    if (modelUrl) {
      useGLTF.preload(modelUrl);
    }
  }, [modelUrl]);

  if (!modelUrl) return null;

  return (
    <Canvas shadows camera={{ position: [5, 5, 5], fov: 75 }}>
      {/* Adaptive resolution */}
      <AdaptiveDpr pixelated />

      <Suspense fallback={<Loader />}>
        <Environment preset="warehouse" />
        <Bounds fit clip margin={2}>
          <Model url={modelUrl} onLoad={setBounds} />
        </Bounds>
        {bounds && showGrid && !awaitingResults && <VoxelGrid />}
        {bounds && <SourcePlacer isStaging={isStaging} />}
      </Suspense>
      {/* Orientation gizmo */}
      <GizmoHelper alignment="bottom-right" margin={[80, 80]}>
        <GizmoViewport
          axisColors={["red", "green", "blue"]}
          labelColor="black"
        />
      </GizmoHelper>
      <OrbitControls makeDefault />
    </Canvas>
  );
}
