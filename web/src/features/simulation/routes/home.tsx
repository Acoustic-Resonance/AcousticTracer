import { useState } from "react";
import { Link } from "react-router";
import { useUser } from "@/features/auth/context/user-store";
import {
  LayoutDashboard,
  Box,
  ArrowRight,
  Home as HomeIcon,
  LogOut,
  LogIn,
  UserPlus,
  Waves,
} from "lucide-react";
import { Sidebar, SidebarBody, SidebarLink } from "@/components/ui/sidebar";
import { motion } from "framer-motion";

export default function Home() {
  const { current, isLoading, logout } = useUser();
  const [sidebarOpen, setSidebarOpen] = useState(false);

  if (isLoading) {
    return (
      <div className="flex h-screen items-center justify-center bg-bg-primary text-text-primary">
        Loading…
      </div>
    );
  }

  /* ───── Guest (not logged in) ───── */
  if (!current) {
    return (
      <div className="flex min-h-screen flex-col items-center justify-center bg-bg-primary px-6 text-text-primary">
        {/* Hero */}
        <div className="flex flex-col items-center gap-4">
          <div className="flex h-14 w-14 items-center justify-center rounded-2xl bg-accent/10">
            <Waves className="h-7 w-7 text-accent" />
          </div>
          <h1 className="text-center text-4xl font-bold sm:text-5xl">
            Acoustic Tracer
          </h1>
          <p className="mt-2 max-w-md text-center leading-relaxed text-text-secondary">
            Simulate and visualise acoustic ray tracing on 3D models. Upload a
            GLB file, configure parameters, and explore the results in an
            interactive 3D scene.
          </p>
        </div>

        {/* Auth CTA */}
        <div className="mt-10 flex flex-col items-center gap-3 sm:flex-row">
          <Link
            to="/auth/login"
            className="inline-flex items-center gap-2 rounded-lg bg-button-primary px-6 py-3 font-medium text-white no-underline transition-colors hover:bg-button-primary/90"
          >
            <LogIn className="h-4 w-4" />
            Log In
          </Link>
          <Link
            to="/auth/register"
            className="inline-flex items-center gap-2 rounded-lg border border-border-primary bg-bg-card px-6 py-3 font-medium text-text-primary no-underline transition-colors hover:border-button-primary/50"
          >
            <UserPlus className="h-4 w-4" />
            Create an Account
          </Link>
        </div>

        <p className="mt-16 text-xs text-text-secondary/60">
          Built with React, Three.js &amp; Tailwind CSS
        </p>
      </div>
    );
  }

  /* ───── Authenticated ───── */
  return (
    <div className="flex h-screen bg-bg-primary text-text-primary overflow-hidden">
      {/* Sidebar */}
      <Sidebar open={sidebarOpen} setOpen={setSidebarOpen}>
        <SidebarBody className="justify-between gap-10">
          <div className="flex flex-col flex-1 overflow-y-auto overflow-x-hidden">
            {sidebarOpen ? <Logo /> : <LogoIcon />}
            <div className="mt-8 flex flex-col gap-2">
              <SidebarLink
                link={{
                  label: "Home",
                  href: "/",
                  icon: (
                    <HomeIcon className="text-text-secondary h-5 w-5 shrink-0" />
                  ),
                }}
              />
              <SidebarLink
                link={{
                  label: "Dashboard",
                  href: "/dashboard",
                  icon: (
                    <LayoutDashboard className="text-text-secondary h-5 w-5 shrink-0" />
                  ),
                }}
              />
            </div>
          </div>
          <div className="flex flex-col gap-2 border-t border-border-primary pt-4">
            <SidebarLink
              link={{
                label: "Logout",
                href: "#",
                icon: (
                  <LogOut className="text-text-secondary h-5 w-5 shrink-0" />
                ),
              }}
              onClick={logout}
            />
            <SidebarLink
              link={{
                label: current?.name ?? "User",
                href: "#",
                icon: (
                  <div className="h-7 w-7 shrink-0 rounded-full bg-accent flex items-center justify-center text-bg-primary font-bold text-xs">
                    {current?.name?.charAt(0).toUpperCase() || "?"}
                  </div>
                ),
              }}
            />
          </div>
        </SidebarBody>
      </Sidebar>

      {/* Main Content */}
      <div className="flex-1 flex flex-col items-center justify-center overflow-y-auto px-6 py-16">
        {/* Welcome */}
        <h1 className="text-center text-4xl font-bold text-text-primary sm:text-5xl capitalize">
          Welcome{current?.name ? `, ${current.name}` : ""}
        </h1>
        <p className="mt-4 max-w-lg text-center text-text-secondary leading-relaxed">
          Simulate and visualise acoustic ray tracing on 3D models. Upload a GLB
          file, configure your simulation parameters, and explore the results in
          an interactive 3D scene.
        </p>

        {/* Navigation cards */}
        <div className="mt-12 grid w-full max-w-2xl gap-4 sm:grid-cols-2">
          <Link
            to="/dashboard"
            className="group flex flex-col gap-3 rounded-xl border border-border-primary bg-bg-card p-6 no-underline transition-all hover:border-button-primary/50 hover:shadow-lg hover:shadow-button-primary/5"
          >
            <div className="flex items-center justify-between">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-button-primary/10">
                <LayoutDashboard className="h-5 w-5 text-button-primary" />
              </div>
              <ArrowRight className="h-4 w-4 text-text-secondary opacity-0 transition-all group-hover:translate-x-1 group-hover:opacity-100" />
            </div>
            <div>
              <h2 className="text-lg font-semibold text-text-primary">
                Dashboard
              </h2>
              <p className="mt-1 text-sm text-text-secondary">
                View, manage, and create new acoustic simulations.
              </p>
            </div>
          </Link>

          <Link
            to="/scene/new"
            className="group flex flex-col gap-3 rounded-xl border border-border-primary bg-bg-card p-6 no-underline transition-all hover:border-accent/50 hover:shadow-lg hover:shadow-accent/5"
          >
            <div className="flex items-center justify-between">
              <div className="flex h-10 w-10 items-center justify-center rounded-lg bg-accent/10">
                <Box className="h-5 w-5 text-accent" />
              </div>
              <ArrowRight className="h-4 w-4 text-text-secondary opacity-0 transition-all group-hover:translate-x-1 group-hover:opacity-100" />
            </div>
            <div>
              <h2 className="text-lg font-semibold text-text-primary">
                Scene Viewer
              </h2>
              <p className="mt-1 text-sm text-text-secondary">
                Open the 3D scene viewer to upload and explore a model.
              </p>
            </div>
          </Link>
        </div>

        {/* Footer hint */}
        <p className="mt-16 text-xs text-text-secondary/60">
          Built with React, Three.js &amp; Tailwind CSS
        </p>
      </div>
    </div>
  );
}

const Logo = () => (
  <div className="font-normal flex space-x-2 items-center text-sm text-text-primary py-1 relative z-20">
    <div className="h-5 w-6 bg-accent rounded-br-lg rounded-tr-sm rounded-tl-lg rounded-bl-sm shrink-0" />
    <motion.span
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      className="font-medium text-text-primary whitespace-pre"
    >
      Acoustic Tracer
    </motion.span>
  </div>
);

const LogoIcon = () => (
  <div className="font-normal flex space-x-2 items-center text-sm text-text-primary py-1 relative z-20">
    <div className="h-5 w-6 bg-accent rounded-br-lg rounded-tr-sm rounded-tl-lg rounded-bl-sm shrink-0" />
  </div>
);
