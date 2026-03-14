import { useState, useRef, useEffect, useMemo, type RefObject } from "react";
import { useFrame } from "@react-three/fiber";
import { Line, TransformControls } from "@react-three/drei";
import type { TransformControls as TransformControlsImpl } from "three-stdlib";
import { useSceneStore } from "../stores/scene-store";
import * as THREE from "three";

type Axis = "all" | "x" | "y" | "z";
type ActiveHandle = "source" | "direction" | null;

export default function SourcePlacer({
  isStaging = false,
}: {
  isStaging?: boolean;
}) {
  const bounds = useSceneStore((s) => s.bounds);
  const sourceX = useSceneStore((s) => s.config.selectedSource.position.x);
  const sourceY = useSceneStore((s) => s.config.selectedSource.position.y);
  const sourceZ = useSceneStore((s) => s.config.selectedSource.position.z);
  const dirX = useSceneStore((s) => s.config.selectedSource.direction.x);
  const dirY = useSceneStore((s) => s.config.selectedSource.direction.y);
  const dirZ = useSceneStore((s) => s.config.selectedSource.direction.z);
  const setSelectedSource = useSceneStore((s) => s.setSelectedSource);

  const controlsRef = useRef<TransformControlsImpl>(null);
  const draggingRef = useRef(false);
  const [activeAxis, setActiveAxis] = useState<Axis>("all");
  const [controlsEnabled, setControlsEnabled] = useState(true);
  const [activeHandle, setActiveHandle] = useState<ActiveHandle>("source");

  // Sync store to TransformControls (not the mesh) when NOT dragging
  useEffect(() => {
    const controls = controlsRef.current;
    if (!controls || draggingRef.current) return;
    const obj = (controls as any).object as THREE.Object3D | undefined;
    if (!obj) return;
    obj.position.set(sourceX, sourceY, sourceZ);
  }, [sourceX, sourceY, sourceZ]);

  // Clamp helper — keeps a position inside the bounding box
  const clamp = (pos: THREE.Vector3, box: THREE.Box3) => {
    pos.x = Math.max(box.min.x, Math.min(box.max.x, pos.x));
    pos.y = Math.max(box.min.y, Math.min(box.max.y, pos.y));
    pos.z = Math.max(box.min.z, Math.min(box.max.z, pos.z));
  };

  // While dragging, clamp the object position to bounds every frame
  useFrame(() => {
    if (!draggingRef.current || !bounds) return;
    const controls = controlsRef.current;
    if (!controls) return;
    const obj = (controls as any).object as THREE.Object3D | undefined;
    if (!obj) return;
    clamp(obj.position, bounds);
  });

  // Listen for drag end — clamp + commit position to store
  useEffect(() => {
    const controls = controlsRef.current;
    if (!controls) return;

    const onDragChanged = (event: { value: boolean }) => {
      draggingRef.current = event.value;

      if (!event.value) {
        const obj = (controls as any).object as THREE.Object3D | undefined;
        if (!obj) return;
        const currentBounds = useSceneStore.getState().bounds;
        if (currentBounds) clamp(obj.position, currentBounds);
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

  // Keybinds: X = X axis, Y = Y axis, Z = Z axis, Escape = all axes
  // Pressing the same key again toggles back to "all".
  useEffect(() => {
    const onKeyDown = (e: KeyboardEvent) => {
      const tag = (e.target as HTMLElement)?.tagName;
      if (tag === "INPUT" || tag === "TEXTAREA" || tag === "SELECT") return;

      switch (e.key.toLowerCase()) {
        case "x":
          setActiveAxis((prev) => (prev === "x" ? "all" : "x"));
          break;
        case "y":
          setActiveAxis((prev) => (prev === "y" ? "all" : "y"));
          break;
        case "z":
          setActiveAxis((prev) => (prev === "z" ? "all" : "z"));
          break;
        case "escape":
          setActiveAxis("all");
          break;
        case "h":
          setControlsEnabled((prev) => !prev);
          break;
        case "tab":
          e.preventDefault();
          setActiveHandle((prev) =>
            prev === "source" ? "direction" : "source",
          );
          break;
      }
    };
    window.addEventListener("keydown", onKeyDown);
    return () => window.removeEventListener("keydown", onKeyDown);
  }, []);

  // Scale factor based on model size — 2% of the diagonal, clamped so the
  // markers stay visible on tiny models but don't dominate large ones.
  const { markerScale, gizmoSize } = useMemo(() => {
    if (!bounds) return { markerScale: 1, gizmoSize: 1 };
    const size = new THREE.Vector3();
    bounds.getSize(size);
    const diagonal = size.length();
    const s = Math.min(Math.max(diagonal * 0.02, 0.02), 2);
    // TransformControls gizmo size — smaller for larger models so the
    // handles don't overshadow the scene.
    const g = Math.min(Math.max(0.4, 1.5 / Math.max(diagonal, 1)), 1);
    return { markerScale: s, gizmoSize: g };
  }, [bounds]);

  const lineLength = markerScale * 3;

  // Normalized direction vector (fallback to -Z)
  const dirVec = useMemo(() => {
    const v = new THREE.Vector3(dirX, dirY, dirZ);
    return v.lengthSq() < 1e-4 ? new THREE.Vector3(0, 0, -1) : v.normalize();
  }, [dirX, dirY, dirZ]);

  const coneQuat = useMemo(() => {
    const q = new THREE.Quaternion();
    q.setFromUnitVectors(new THREE.Vector3(0, 1, 0), dirVec);
    return q;
  }, [dirVec]);

  if (!bounds) return null;

  const markerSize = markerScale;
  const coneRadius = markerScale * 0.6;
  const coneHeight = coneRadius * 2.5;

  const sourceActive = activeHandle === "source";

  const marker = (
    <mesh
      renderOrder={999}
      onPointerDown={(e) => {
        if (isStaging && activeHandle !== "source") {
          e.stopPropagation();
          setActiveHandle("source");
        }
      }}
    >
      <sphereGeometry args={[markerSize, 16, 16]} />
      <meshBasicMaterial
        color={0xff0000}
        transparent
        opacity={isStaging && !sourceActive ? 0.5 : 1}
        depthTest={false}
      />
    </mesh>
  );

  if (!isStaging) {
    return (
      <group position={[sourceX, sourceY, sourceZ]}>
        {marker}
        <Line
          points={[
            [0, 0, 0],
            [
              dirVec.x * lineLength,
              dirVec.y * lineLength,
              dirVec.z * lineLength,
            ],
          ]}
          color={0xffaa00}
          lineWidth={2}
          renderOrder={998}
        />
        <mesh
          position={[
            dirVec.x * lineLength,
            dirVec.y * lineLength,
            dirVec.z * lineLength,
          ]}
          quaternion={coneQuat}
          renderOrder={999}
        >
          <coneGeometry args={[coneRadius, coneHeight, 8]} />
          <meshBasicMaterial
            color={0xffaa00}
            transparent
            opacity={0.9}
            depthTest={false}
          />
        </mesh>
      </group>
    );
  }

  return (
    <>
      <TransformControls
        ref={controlsRef}
        mode="translate"
        size={gizmoSize}
        enabled={controlsEnabled && sourceActive}
        showX={
          controlsEnabled &&
          sourceActive &&
          (activeAxis === "all" || activeAxis === "x")
        }
        showY={
          controlsEnabled &&
          sourceActive &&
          (activeAxis === "all" || activeAxis === "y")
        }
        showZ={
          controlsEnabled &&
          sourceActive &&
          (activeAxis === "all" || activeAxis === "z")
        }
      >
        {marker}
      </TransformControls>
      <DirectionArrow
        sourceControlsRef={controlsRef}
        lineLength={lineLength}
        coneRadius={coneRadius}
        coneHeight={coneHeight}
        gizmoSize={gizmoSize}
        controlsEnabled={controlsEnabled}
        activeHandle={activeHandle}
        setActiveHandle={setActiveHandle}
      />
    </>
  );
}

function DirectionArrow({
  sourceControlsRef,
  lineLength,
  coneRadius,
  coneHeight,
  gizmoSize,
  controlsEnabled,
  activeHandle,
  setActiveHandle,
}: {
  sourceControlsRef: RefObject<TransformControlsImpl | null>;
  lineLength: number;
  coneRadius: number;
  coneHeight: number;
  gizmoSize: number;
  controlsEnabled: boolean;
  activeHandle: ActiveHandle;
  setActiveHandle: (h: ActiveHandle) => void;
}) {
  const setSelectedSource = useSceneStore((s) => s.setSelectedSource);

  const controlsRef = useRef<TransformControlsImpl>(null);
  const coneRef = useRef<THREE.Mesh>(null);
  const draggingRef = useRef(false);

  // Line object — created once, vertex positions updated per-frame
  const line = useMemo(() => {
    const geom = new THREE.BufferGeometry();
    geom.setAttribute(
      "position",
      new THREE.Float32BufferAttribute(new Float32Array(6), 3),
    );
    const mat = new THREE.LineBasicMaterial({
      color: 0xffaa00,
      depthTest: false,
    });
    const l = new THREE.Line(geom, mat);
    l.renderOrder = 998;
    l.frustumCulled = false;
    return l;
  }, []);

  // Dispose geometry + material on unmount
  useEffect(() => {
    return () => {
      line.geometry.dispose();
      (line.material as THREE.Material).dispose();
    };
  }, [line]);

  const dir = useMemo(() => new THREE.Vector3(), []);
  const yUp = useMemo(() => new THREE.Vector3(0, 1, 0), []);
  const quat = useMemo(() => new THREE.Quaternion(), []);

  // Per-frame: follow source, update line endpoints, orient cone
  useFrame(() => {
    // Live source position (reads from parent TransformControls during drag)
    const srcCtrl = sourceControlsRef.current;
    const srcObj = srcCtrl
      ? ((srcCtrl as any).object as THREE.Object3D | undefined)
      : undefined;

    let sx: number, sy: number, sz: number;
    if (srcObj) {
      sx = srcObj.position.x;
      sy = srcObj.position.y;
      sz = srcObj.position.z;
    } else {
      const p = useSceneStore.getState().config.selectedSource.position;
      sx = p.x;
      sy = p.y;
      sz = p.z;
    }

    const ctrl = controlsRef.current;
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const handleObj = ctrl
      ? ((ctrl as any).object as THREE.Object3D | undefined)
      : undefined;

    // When NOT dragging the direction handle, lock it to source + dir * length
    if (!draggingRef.current && handleObj) {
      const d = useSceneStore.getState().config.selectedSource.direction;
      dir.set(d.x, d.y, d.z);
      if (dir.lengthSq() < 1e-4) dir.set(0, 0, -1);
      dir.normalize();
      handleObj.position.set(
        sx + dir.x * lineLength,
        sy + dir.y * lineLength,
        sz + dir.z * lineLength,
      );
    }

    // Update line endpoints
    const posAttr = line.geometry.getAttribute(
      "position",
    ) as THREE.BufferAttribute;
    const buf = posAttr.array as Float32Array;
    buf[0] = sx;
    buf[1] = sy;
    buf[2] = sz;
    if (handleObj) {
      buf[3] = handleObj.position.x;
      buf[4] = handleObj.position.y;
      buf[5] = handleObj.position.z;
    }
    posAttr.needsUpdate = true;

    // Orient cone so its tip points away from the source
    if (coneRef.current && handleObj) {
      dir.set(
        handleObj.position.x - sx,
        handleObj.position.y - sy,
        handleObj.position.z - sz,
      );
      if (dir.lengthSq() > 1e-4) {
        dir.normalize();
        quat.setFromUnitVectors(yUp, dir);
        coneRef.current.quaternion.copy(quat);
      }
    }
  });

  // On drag end: normalize direction, snap handle, commit to store
  useEffect(() => {
    const ctrl = controlsRef.current;
    if (!ctrl) return;

    const onDragChanged = (event: { value: boolean }) => {
      draggingRef.current = event.value;

      if (!event.value) {
        const obj = (ctrl as any).object as THREE.Object3D | undefined;
        if (!obj) return;

        const { position: src } =
          useSceneStore.getState().config.selectedSource;
        const newDir = new THREE.Vector3(
          obj.position.x - src.x,
          obj.position.y - src.y,
          obj.position.z - src.z,
        );
        if (newDir.lengthSq() < 1e-4) newDir.set(0, 0, -1);
        newDir.normalize();

        // Snap handle back to fixed distance from source
        obj.position.set(
          src.x + newDir.x * lineLength,
          src.y + newDir.y * lineLength,
          src.z + newDir.z * lineLength,
        );

        setSelectedSource(src, {
          x: newDir.x,
          y: newDir.y,
          z: newDir.z,
        });
      }
    };

    (ctrl as any).addEventListener("dragging-changed", onDragChanged);
    return () => {
      (ctrl as any).removeEventListener("dragging-changed", onDragChanged);
    };
  }, [setSelectedSource, lineLength]);

  return (
    <>
      <primitive object={line} />
      <TransformControls
        ref={controlsRef}
        mode="translate"
        size={gizmoSize * 0.6}
        enabled={controlsEnabled && activeHandle === "direction"}
        showX={controlsEnabled && activeHandle === "direction"}
        showY={controlsEnabled && activeHandle === "direction"}
        showZ={controlsEnabled && activeHandle === "direction"}
      >
        <mesh
          ref={coneRef}
          renderOrder={999}
          onPointerDown={(e) => {
            if (activeHandle !== "direction") {
              e.stopPropagation();
              setActiveHandle("direction");
            }
          }}
        >
          <coneGeometry args={[coneRadius, coneHeight, 8]} />
          <meshBasicMaterial
            color={0xffaa00}
            transparent
            opacity={activeHandle === "direction" ? 0.9 : 0.4}
            depthTest={false}
          />
        </mesh>
      </TransformControls>
    </>
  );
}
