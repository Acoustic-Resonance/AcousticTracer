import { UserProvider } from "@/features/auth/context/user-store";
import React, { Suspense } from "react";
import { ErrorBoundary } from "react-error-boundary";
import { MainErrorFallback } from "@/components/main-error-fallback";
import { QueryClientProvider } from "@tanstack/react-query";
import { queryClient } from "@/lib/query-client";
import ToastContainer from "@/components/toast";

interface AppProviderProps {
  children: React.ReactNode;
}

export const AppProvider = ({ children }: AppProviderProps) => {
  return (
    <ErrorBoundary FallbackComponent={MainErrorFallback}>
      <Suspense
        fallback={
          <div className="flex h-screen items-center justify-center">
            Loading...
          </div>
        }
      >
        <QueryClientProvider client={queryClient}>
          <UserProvider>
            {children}
            <ToastContainer />
          </UserProvider>
        </QueryClientProvider>
      </Suspense>
    </ErrorBoundary>
  );
};
