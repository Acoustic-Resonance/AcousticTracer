import { UserProvider, useUser } from "./lib/context/user";
import Home from "./pages/Home";
import Login from "./pages/Login";
import "./Login.css";
import "./App.css";

function AppContent() {
  const { current, isLoading } = useUser();

  if (isLoading) {
    return <div className="loading-container">Loading...</div>;
  }

  return <>{current ? <Home /> : <Login />}</>;
}

export default function App() {
  return (
    <UserProvider>
      <AppContent />
    </UserProvider>
  );
}
