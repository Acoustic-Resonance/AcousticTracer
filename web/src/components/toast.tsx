import { useState, useEffect, useCallback } from "react";

export interface ToastMessage {
  id: string;
  text: string;
  type?: "success" | "error" | "info";
  duration?: number;
}

let toastListener: ((toast: ToastMessage) => void) | null = null;

/** Fire-and-forget toast from anywhere — no context needed. */
export function showToast(
  text: string,
  type: ToastMessage["type"] = "success",
  duration = 2500,
) {
  const id = crypto.randomUUID();
  toastListener?.({ id, text, type, duration });
}

export default function ToastContainer() {
  const [toasts, setToasts] = useState<ToastMessage[]>([]);

  useEffect(() => {
    toastListener = (toast) => setToasts((prev) => [...prev, toast]);
    return () => {
      toastListener = null;
    };
  }, []);

  const remove = useCallback((id: string) => {
    setToasts((prev) => prev.filter((t) => t.id !== id));
  }, []);

  return (
    <div
      aria-live="polite"
      className="fixed top-6 left-1/2 z-9999 flex -translate-x-1/2 flex-col items-center gap-2 pointer-events-none"
    >
      {toasts.map((toast) => (
        <Toast key={toast.id} toast={toast} onDone={remove} />
      ))}
    </div>
  );
}

function Toast({
  toast,
  onDone,
}: {
  toast: ToastMessage;
  onDone: (id: string) => void;
}) {
  const [visible, setVisible] = useState(false);

  useEffect(() => {
    const enterFrame = requestAnimationFrame(() => setVisible(true));

    const hideTimer = setTimeout(() => {
      setVisible(false);
    }, toast.duration ?? 2500);

    // Remove from DOM after exit animation completes
    const removeTimer = setTimeout(
      () => onDone(toast.id),
      (toast.duration ?? 2500) + 300,
    );

    return () => {
      cancelAnimationFrame(enterFrame);
      clearTimeout(hideTimer);
      clearTimeout(removeTimer);
    };
  }, [toast, onDone]);

  const bgColor =
    toast.type === "error"
      ? "bg-danger/90 border-danger/40"
      : toast.type === "info"
        ? "bg-button-primary/90 border-button-primary/40"
        : "bg-success/90 border-success/40";

  const icon =
    toast.type === "error" ? (
      <svg
        className="size-4 shrink-0"
        fill="none"
        viewBox="0 0 24 24"
        stroke="currentColor"
        strokeWidth={2}
      >
        <path
          strokeLinecap="round"
          strokeLinejoin="round"
          d="M6 18L18 6M6 6l12 12"
        />
      </svg>
    ) : toast.type === "info" ? (
      <svg
        className="size-4 shrink-0"
        fill="none"
        viewBox="0 0 24 24"
        stroke="currentColor"
        strokeWidth={2}
      >
        <path
          strokeLinecap="round"
          strokeLinejoin="round"
          d="M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
        />
      </svg>
    ) : (
      <svg
        className="size-4 shrink-0"
        fill="none"
        viewBox="0 0 24 24"
        stroke="currentColor"
        strokeWidth={2}
      >
        <path strokeLinecap="round" strokeLinejoin="round" d="M5 13l4 4L19 7" />
      </svg>
    );

  return (
    <div
      className={`pointer-events-auto flex items-center gap-2 rounded-lg border px-4 py-2.5 text-sm font-medium text-white shadow-lg backdrop-blur-sm transition-all duration-300 ease-out ${bgColor} ${
        visible
          ? "translate-y-0 scale-100 opacity-100"
          : "-translate-y-2 scale-95 opacity-0"
      }`}
    >
      {icon}
      <span>{toast.text}</span>
    </div>
  );
}
