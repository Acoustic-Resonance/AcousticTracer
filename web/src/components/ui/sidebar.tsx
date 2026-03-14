import { cn } from "@/lib/utils";
import { Link, type LinkProps } from "react-router";
import { useState, createContext, useContext, type ReactNode } from "react";
import { motion } from "framer-motion";

interface Links {
  label: string;
  href: string;
  icon: ReactNode;
}

interface SidebarContextProps {
  open: boolean;
  setOpen: React.Dispatch<React.SetStateAction<boolean>>;
}

const SidebarContext = createContext<SidebarContextProps | undefined>(
  undefined,
);

const useSidebar = () => {
  const context = useContext(SidebarContext);
  if (!context) {
    throw new Error("useSidebar must be used within a Sidebar");
  }
  return context;
};

export const Sidebar = ({
  children,
  open: openProp,
  setOpen: setOpenProp,
}: {
  children: ReactNode;
  open?: boolean;
  setOpen?: React.Dispatch<React.SetStateAction<boolean>>;
}) => {
  const [openState, setOpenState] = useState(false);
  const open = openProp ?? openState;
  const setOpen = setOpenProp ?? setOpenState;

  return (
    <SidebarContext.Provider value={{ open, setOpen }}>
      {children}
    </SidebarContext.Provider>
  );
};

export const SidebarBody = ({
  className,
  children,
  ...props
}: React.ComponentProps<typeof motion.div>) => {
  const { open, setOpen } = useSidebar();

  return (
    <motion.div
      className={cn(
        "h-full px-4 py-4 flex flex-col bg-bg-card w-75 shrink-0 border-r border-border-primary overflow-hidden",
        className,
      )}
      initial={false}
      animate={{ width: open ? "300px" : "60px" }}
      transition={{ duration: 0.3, ease: "easeInOut" }}
      onMouseEnter={() => setOpen(true)}
      onMouseLeave={() => setOpen(false)}
      {...props}
    >
      {children}
    </motion.div>
  );
};

export const SidebarLink = ({
  link,
  className,
  onClick,
  ...props
}: {
  link: Links;
  className?: string;
  onClick?: () => void;
  props?: Omit<LinkProps, "to">;
}) => {
  const { open } = useSidebar();

  const label = (
    <motion.span
      animate={{
        opacity: open ? 1 : 0,
        width: open ? "auto" : 0,
      }}
      transition={{ duration: 0.2, ease: "easeInOut" }}
      className="text-text-secondary text-sm group-hover/sidebar:translate-x-1 transition-transform duration-150 whitespace-pre overflow-hidden p-0! m-0!"
    >
      {link.label}
    </motion.span>
  );

  if (onClick) {
    return (
      <button
        type="button"
        className={cn(
          "flex items-center justify-start gap-2 group/sidebar py-2 bg-transparent border-none cursor-pointer w-full text-left",
          className,
        )}
        onClick={onClick}
      >
        {link.icon}
        {label}
      </button>
    );
  }

  return (
    <Link
      to={link.href}
      className={cn(
        "flex items-center justify-start gap-2 group/sidebar py-2 no-underline",
        className,
      )}
      {...props}
    >
      {link.icon}
      {label}
    </Link>
  );
};
