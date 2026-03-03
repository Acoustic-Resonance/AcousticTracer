import {
  createBrowserRouter,
  RouterProvider,
  Outlet,
  Navigate,
} from "react-router";
import { lazy } from "react";
import { useUser } from "@/features/auth/context/user-store";
import { FeatureErrorFallback } from "@/components/feature-error-boundary";
import { ErrorBoundary } from "react-error-boundary";
// Lazy loading the different pages

const Dashboard = lazy(() => import("@/features/simulation/routes/dashboard"));
const Scene = lazy(() => import("@/features/simulation/routes/scene"));
const Login = lazy(() => import("@/features/auth/routes/login"));

const ProtectedRoute = () => {
  const { current, isLoading } = useUser();

  if (isLoading) return null;

  if (!current) return <Navigate to="/auth/login" replace />;
  return <Outlet />;
};

const router = createBrowserRouter([
  {
    // Protected routes
    path: "/",
    element: <ProtectedRoute />,
    children: [
      {
        index: true,
        element: (
          <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
            <Navigate to="/dashboard" replace />
          </ErrorBoundary>
        ),
      },
      {
        path: "dashboard",
        element: (
          <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
            <Dashboard />
          </ErrorBoundary>
        ),
      },
      {
        path: "scene/:idOfFile",
        element: (
          <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
            <Scene />
          </ErrorBoundary>
        ),
      },
      {
        path: "scene",
        element: (
          <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
            <Navigate to="scene/new" replace />
          </ErrorBoundary>
        ),
      },
    ],
  },
  // Public routes
  {
    path: "auth/login",
    element: (
      <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
        <Login />
      </ErrorBoundary>
    ),
  },
  {
    path: "*",
    element: <div className="p-10 text-center">Error 404- Page Not Found</div>,
  },
]);

export const AppRouter = () => {
  return <RouterProvider router={router} />;
};
