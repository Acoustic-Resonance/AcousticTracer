import { ID, OAuthProvider, type Models } from "appwrite";
import {
  createContext,
  useContext,
  useEffect,
  useState,
  type ReactNode,
} from "react";
import { queryClient } from "@/lib/query-client";
import { account } from "@/lib/appwrite";
import { useSceneStore } from "@/features/simulation/stores/scene-store";
interface UserContextType {
  current: Models.User<Models.Preferences> | null;
  isLoading: boolean;
  login: (email: string, password: string) => Promise<void>;
  logout: () => Promise<void>;
  loginWithGoogle: () => void;
  register: (email: string, password: string, name: string) => Promise<void>;
  refreshUser: () => Promise<void>;
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

  function loginWithGoogle() {
    account.createOAuth2Token({
      provider: OAuthProvider.Google,
      success: `${window.location.origin}/`,
      failure: `${window.location.origin}/auth/login`,
    });
  }

  async function login(email: string, password: string) {
    try {
      await account.createEmailPasswordSession({
        email: email,
        password: password,
      });
    } catch (err: unknown) {
      // Appwrite throws a 401 with type "user_session_already_exists"
      const isSessionConflict =
        typeof err === "object" &&
        err !== null &&
        "type" in err &&
        (err as { type: unknown }).type === "user_session_already_exists";
      if (isSessionConflict) {
        throw new Error(
          "You are already logged in. Please log out first or go to the",
        );
      }
      throw err;
    }
    useSceneStore.getState().reset();
    const loggedIn = await account.get();
    setUser(loggedIn);
  }

  async function logout() {
    await account.deleteSession({ sessionId: "current" });
    queryClient.clear();
    useSceneStore.getState().reset();
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

  async function refreshUser() {
    const updated = await account.get();
    setUser(updated);
  }

  async function init() {
    try {
      // Handle OAuth callback: exchange token for session
      const params = new URLSearchParams(window.location.search);
      const secret = params.get("secret");
      const userId = params.get("userId");

      if (secret && userId) {
        try {
          await account.createSession({ userId, secret });
        } catch {}
        // Clean the URL so the token isn't visible / reused
        window.history.replaceState({}, "", window.location.pathname);
      }

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
      value={{
        current: user,
        isLoading,
        login,
        logout,
        register,
        loginWithGoogle,
        refreshUser,
      }}
    >
      {children}
    </UserContext.Provider>
  );
}
