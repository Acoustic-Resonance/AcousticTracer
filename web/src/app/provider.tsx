import { UserProvider } from "@/features/auth/context/user-store";
import React, { Suspense } from "react";
import { ErrorBoundary } from "react-error-boundary";
import { MainErrorFallback } from "@/components/main-error-fallback";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { ReactQueryDevtools } from "@tanstack/react-query-devtools";
export const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      staleTime: 1000 * 60 * 5,
      gcTime: 1000 * 60 * 30,
      retry: 1,
      refetchOnWindowFocus: false,
    },
    mutations: {
      retry: 0,
    },
  },
});

interface AppProviderProps {
  children: React.ReactNode;
}

export const AppProvider = ({ children }: AppProviderProps) => {
  return (
    <ErrorBoundary FallbackComponent={MainErrorFallback}>
      <Suspense
        fallback={
          <div className="flex h-screen items-center justify-center">
            Loading App.
          </div>
        }
      >
        <QueryClientProvider client={queryClient}>
          <UserProvider>{children}</UserProvider>
          <ReactQueryDevtools initialIsOpen={false} />
        </QueryClientProvider>
      </Suspense>
    </ErrorBoundary>
  );
};
