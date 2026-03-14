import { useState } from "react";
import { useUser } from "@/features/auth/context/user-store";
import { Sidebar, SidebarBody, SidebarLink } from "@/components/ui/sidebar";
import { LayoutDashboard, LogOut, Home, Settings, Waves, Box } from "lucide-react";
import { motion } from "framer-motion";

export default function AppSidebar({
  children,
}: {
  children: React.ReactNode;
}) {
  const { current, logout } = useUser();
  const [sidebarOpen, setSidebarOpen] = useState(false);

  return (
    <div className="flex h-screen bg-bg-primary text-text-primary overflow-hidden">
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
                    <Home className="text-text-secondary h-5 w-5 shrink-0" />
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
              <SidebarLink
                link={{
                  label: "Scene Viewer",
                  href: "/scene/new",
                  icon: (
                    <Box className="text-text-secondary h-5 w-5 shrink-0" />
                  ),
                }}
              />
              <SidebarLink
                link={{
                  label: "Settings",
                  href: "/settings",
                  icon: (
                    <Settings className="text-text-secondary h-5 w-5 shrink-0" />
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
                href: "/settings",
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
      {children}
    </div>
  );
}

const Logo = () => (
  <div className="font-normal flex space-x-2 items-center text-sm text-text-primary py-1 relative z-20">
    <Waves className="h-5 w-6 text-accent shrink-0" />
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
    <Waves className="h-5 w-6 text-accent shrink-0" />
  </div>
);
