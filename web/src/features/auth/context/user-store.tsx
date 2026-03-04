import { ID, type Models } from "appwrite";
import {
  createContext,
  useContext,
  useEffect,
  useState,
  type ReactNode,
} from "react";
import { account } from "@/lib/appwrite";
interface UserContextType {
  current: Models.User<Models.Preferences> | null;
  isLoading: boolean;
  login: (email: string, password: string) => Promise<void>;
  logout: () => Promise<void>;
  register: (email: string, password: string, name: string) => Promise<void>;
}

const UserContext = createContext<UserContextType | undefined>(undefined);

export function useUser() {
  const context = useContext(UserContext);
  if (context === undefined) {
    throw new Error("useUser must be used within a UserProvider");
  }
  return context;
}

export function UserProvider({ children }: { children: ReactNode }) {
  const [user, setUser] = useState<Models.User<Models.Preferences> | null>(
    null,
  );
  const [isLoading, setIsLoading] = useState(true);

  async function login(email: string, password: string) {
    await account.createEmailPasswordSession({
      email: email,
      password: password,
    });

    const loggedIn = await account.get();
    setUser(loggedIn);
  }

  async function logout() {
    await account.deleteSession({ sessionId: "current" });
    setUser(null);
  }

  async function register(email: string, password: string, name: string) {
    await account.create({
      userId: ID.unique(),
      email: email,
      password: password,
      name: name,
    });
    await login(email, password);
  }

  async function init() {
    try {
      const loggedIn = await account.get();
      setUser(loggedIn);
    } catch (err: unknown) {
      // 401 = not authenticated → expected, go to login.
      // Anything else is a transient/network error → log it.
      const status =
        typeof err === "object" &&
        err !== null &&
        "code" in err &&
        typeof (err as { code: unknown }).code === "number"
          ? (err as { code: number }).code
          : 0;
      if (status !== 0 && status !== 401) {
        console.warn("Auth init failed (non-401):", err);
      }
      setUser(null);
    } finally {
      setIsLoading(false);
    }
  }

  useEffect(() => {
    init();
  }, []);
  return (
    <UserContext.Provider
      value={{ current: user, isLoading, login, logout, register }}
    >
      {children}
    </UserContext.Provider>
  );
}
