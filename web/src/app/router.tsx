import {
  createBrowserRouter,
  RouterProvider,
  Outlet,
  Navigate,
} from "react-router";
import { lazy, Suspense } from "react";
import { useUser } from "@/features/auth/context/user-store";
import { FeatureErrorFallback } from "@/components/feature-error-boundary";
import { ErrorBoundary } from "react-error-boundary";
// Lazy loading the different pages

const Home = lazy(() => import("@/features/simulation/routes/home"));
const Dashboard = lazy(() => import("@/features/simulation/routes/dashboard"));
const Scene = lazy(() => import("@/features/simulation/routes/scene"));
const Login = lazy(() => import("@/features/auth/routes/login"));
const Register = lazy(() => import("@/features/auth/routes/register"));

const ProtectedRoute = () => {
  const { current, isLoading } = useUser();

  if (isLoading) return null;

  if (!current) return <Navigate to="/auth/login" replace />;

  // This Suspense boundary catches lazy-route chunk loads so the
  // outer AppProvider Suspense doesn't unmount the whole tree
  // (which would destroy any active WebGL Canvas and lose context).
  return (
    <Suspense
      fallback={
        <div className="flex h-screen items-center justify-center text-text-primary">
          Loading...
        </div>
      }
    >
      <Outlet />
    </Suspense>
  );
};

const router = createBrowserRouter([
  {
    // Home — public (shows guest CTA when logged out)
    path: "/",
    element: (
      <Suspense
        fallback={
          <div className="flex h-screen items-center justify-center text-text-primary">
            Loading...
          </div>
        }
      >
        <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
          <Home />
        </ErrorBoundary>
      </Suspense>
    ),
  },
  {
    // Protected routes
    path: "/",
    element: <ProtectedRoute />,
    children: [
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
    path: "auth/register",
    element: (
      <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
        <Register />
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
