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
const Settings = lazy(() => import("@/features/auth/routes/settings"));
const ForgotPassword = lazy(
  () => import("@/features/auth/routes/forgot-password"),
);
const ResetPassword = lazy(
  () => import("@/features/auth/routes/reset-password"),
);

const ProtectedRoute = () => {
  const { current, isLoading } = useUser();

  if (isLoading) return null;

  if (!current) return <Navigate to="/" replace />;

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
      {
        path: "settings",
        element: (
          <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
            <Settings />
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
    path: "auth/forgot-password",
    element: (
      <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
        <ForgotPassword />
      </ErrorBoundary>
    ),
  },
  {
    path: "auth/reset-password",
    element: (
      <ErrorBoundary FallbackComponent={FeatureErrorFallback}>
        <ResetPassword />
      </ErrorBoundary>
    ),
  },
  {
    path: "*",
    element: (
      <div className="flex flex-col items-center justify-center h-screen bg-bg-primary text-text-primary gap-4">
        <h1 className="text-6xl font-bold text-text-secondary">404</h1>
        <p className="text-text-secondary">Page not found</p>
        <a
          href="/"
          className="mt-4 px-6 py-2 bg-button-primary text-white rounded-lg no-underline hover:bg-button-hover transition-colors"
        >
          Go Home
        </a>
      </div>
    ),
  },
]);

export const AppRouter = () => {
  return <RouterProvider router={router} />;
};
