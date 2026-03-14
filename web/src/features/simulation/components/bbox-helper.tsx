import { useSceneStore } from "../stores/scene-store";
export default function BoundBoxHelper() {
  const bounds = useSceneStore((state) => state.bounds);

  if (!bounds) return null;

  // React components must return JSX, not raw instances.
  // Using the declarative <box3Helper> lets R3F handle lifecycle and updates efficiently.
  return <box3Helper args={[bounds, 0xffff00]} />;
}
