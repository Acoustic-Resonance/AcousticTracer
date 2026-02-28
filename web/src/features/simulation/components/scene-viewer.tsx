import { Canvas } from "@react-three/fiber";
import {
  OrbitControls,
  useGLTF,
  Bounds,
  Html,
  useProgress,
} from "@react-three/drei";
import { Suspense, useEffect } from "react";
import * as THREE from "three";
import { useSceneStore } from "../stores/scene-store";
import VoxelGrid from "./voxel-grid";
import BoundBoxHelper from "./bbox-helper";
interface SceneCanvasProps {
  modelUrl: string | null;
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
  onLoad: (box: THREE.Box3) => void;
}) {
  // useGLTF automatically caches by URL
  const { scene } = useGLTF(url);

  useEffect(() => {
    if (scene) {
      scene.traverse((child) => {
        // node.position.set(0,0,0);
        // node.rotation.set(0,0,0);
        child.scale.set(1, 1, 1);
        child.updateMatrix();

        if (child instanceof THREE.Mesh) {
          child.material = new THREE.MeshStandardMaterial({
            color: "#888888",
            roughness: 0.8,
            metalness: 0.1,
          });
        }
      });

      scene.updateMatrixWorld(true);

      const rawBox = new THREE.Box3().setFromObject(scene);
      const size = new THREE.Vector3();

      rawBox.getSize(size);
      const maxDim = Math.max(size.x, size.y, size.z);
      // Sponza is ridicously big so if one of dims is really large it scales it down similar to c code
      if (maxDim > 100) {
        const scaleFactor = 1 / Math.round(maxDim / 10);
        scene.scale.set(scaleFactor, scaleFactor, scaleFactor);
        scene.updateMatrixWorld(true);
      }

      const box = new THREE.Box3().setFromObject(scene);
      onLoad(box);
    }
  }, [scene, onLoad]);

  return <primitive object={scene} />;
}

export default function SceneCanvas({ modelUrl }: SceneCanvasProps) {
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
    <Canvas camera={{ position: [5, 5, 5], fov: 75 }}>
      <ambientLight intensity={0.7} />
      <directionalLight position={[10, 10, 5]} intensity={1} />
      <Suspense fallback={<Loader />}>
        <Bounds fit clip observe margin={2}>
          <Model url={modelUrl} onLoad={setBounds} />
          {bounds && showGrid && <VoxelGrid />}
          <BoundBoxHelper />
        </Bounds>
      </Suspense>
      <OrbitControls />
    </Canvas>
  );
}
