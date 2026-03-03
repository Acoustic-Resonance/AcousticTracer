// src/features/simulation/components/source-placer.tsx

import { useState, useRef, useEffect } from "react";
import { TransformControls } from "@react-three/drei";
import type { TransformControls as TransformControlsImpl } from "three-stdlib";
import { useSceneStore } from "../stores/scene-store";
import * as THREE from "three";

type Axis = "all" | "x" | "y" | "z";

export default function SourcePlacer({ isStaging = false }: { isStaging?: boolean }) {
  const bounds = useSceneStore((s) => s.bounds);
  const voxelSize = useSceneStore((s) => s.config.voxelSize);
  const sourceX = useSceneStore((s) => s.config.selectedSource.position.x);
  const sourceY = useSceneStore((s) => s.config.selectedSource.position.y);
  const sourceZ = useSceneStore((s) => s.config.selectedSource.position.z);
  const setSelectedSource = useSceneStore((s) => s.setSelectedSource);

  const controlsRef = useRef<TransformControlsImpl>(null);
  const draggingRef = useRef(false);
  const [activeAxis, setActiveAxis] = useState<Axis>("all");
  const [controlsEnabled, setControlsEnabled] = useState(true);

  // Sync store to TransformControls wrapper group (not the mesh) when NOT dragging
  useEffect(() => {
    const controls = controlsRef.current;
    if (!controls || draggingRef.current) return;
    const obj = (controls as any).object as THREE.Object3D | undefined;
    if (!obj) return;
    obj.position.set(sourceX, sourceY, sourceZ);
  }, [sourceX, sourceY, sourceZ]);

  // Listen for drag end — read position from the wrapper group and commit to store
  useEffect(() => {
    const controls = controlsRef.current;
    if (!controls) return;

    const onDragChanged = (event: { value: boolean }) => {
      draggingRef.current = event.value;

      if (!event.value) {
        const obj = (controls as any).object as THREE.Object3D | undefined;
        if (!obj) return;
        const { x, y, z } = obj.position;
        const dir = useSceneStore.getState().config.selectedSource.direction;
        setSelectedSource({ x, y, z }, dir);
      }
    };

    (controls as any).addEventListener("dragging-changed", onDragChanged);
    return () => {
      (controls as any).removeEventListener("dragging-changed", onDragChanged);
    };
  }, [setSelectedSource]);

  // Keybinds: j = X axis, k = Y axis, l = Z axis, Escape = all axes
  // Pressing the same key again toggles back to "all".
  useEffect(() => {
    const onKeyDown = (e: KeyboardEvent) => {
      const tag = (e.target as HTMLElement)?.tagName;
      if (tag === "INPUT" || tag === "TEXTAREA" || tag === "SELECT") return;

      switch (e.key.toLowerCase()) {
        case "j":
          setActiveAxis((prev) => (prev === "x" ? "all" : "x"));
          break;
        case "k":
          setActiveAxis((prev) => (prev === "y" ? "all" : "y"));
          break;
        case "l":
          setActiveAxis((prev) => (prev === "z" ? "all" : "z"));
          break;
        case "escape":
          setActiveAxis("all");
          break;
        case "h":
          setControlsEnabled((prev) => !prev);
          break;
      }
    };
    window.addEventListener("keydown", onKeyDown);
    return () => window.removeEventListener("keydown", onKeyDown);
  }, []);

  if (!bounds) return null;

  const markerSize = Math.max(0.05, voxelSize * 0.2);

  const marker = (
    <mesh renderOrder={999}>
      <sphereGeometry args={[markerSize, 16, 16]} />
      <meshBasicMaterial
        color={0xFF0000}
        transparent
        opacity={0.95}
        depthTest={false}
      />
    </mesh>
  );

  // Read-only: just show the sphere at the stored position
  if (!isStaging) {
    return (
      <group position={[sourceX, sourceY, sourceZ]}>
        {marker}
      </group>
    );
  }

  return (
    <TransformControls
      ref={controlsRef}
      mode="translate"
      enabled={controlsEnabled}
      showX={controlsEnabled && (activeAxis === "all" || activeAxis === "x")}
      showY={controlsEnabled && (activeAxis === "all" || activeAxis === "y")}
      showZ={controlsEnabled && (activeAxis === "all" || activeAxis === "z")}
    >
      {marker}
    </TransformControls>
  );
}
