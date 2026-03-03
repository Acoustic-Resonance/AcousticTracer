import { useState } from "react";
import { useUser } from "../context/user-store";
import { useNavigate } from "react-router";
export default function Login() {
  const navigate = useNavigate();
  const { login, register } = useUser();
  const [isRegistering, setIsRegistering] = useState(false);
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [name, setName] = useState("");
  const [error, setError] = useState("");

  const handleSubmit = async (e: React.SubmitEvent) => {
    e.preventDefault();
    setError("");
    try {
      if (isRegistering) {
        await register(email, password, name);
      } else {
        await login(email, password);
        navigate("/dashboard");
      }
    } catch (err: unknown) {
      setError(err instanceof Error ? err.message : String(err));
    }
  };

  return (
    <div className="flex items-center justify-center min-h-screen p-4">
      <div className="bg-bg-card rounded-xl shadow-md max-w-md w-full p-10">
        <h1 className="text-center mb-8 text-2xl font-bold text-text-primary">
          {isRegistering ? "Create Account" : "Welcome Back"}
        </h1>

        <form onSubmit={handleSubmit} className="flex flex-col gap-5">
          {isRegistering && (
            <div className="flex flex-col gap-2">
              <label
                className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
                htmlFor="name"
              >
                Name
              </label>
              <input
                id="name"
                className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
                type="text"
                placeholder="Enter your name"
                value={name}
                onChange={(e) => setName(e.target.value)}
                required
              />
            </div>
          )}
          <div className="flex flex-col gap-2">
            <label
              className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
              htmlFor="email"
            >
              Email
            </label>
            <input
              id="email"
              className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
              type="email"
              placeholder="Enter your email"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              required
            />
          </div>

          <div className="flex flex-col gap-2">
            <label
              className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
              htmlFor="password"
            >
              Password
            </label>
            <input
              id="password"
              className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
              type="password"
              placeholder="Enter your password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              required
            />
          </div>

          {error && (
            <div className="text-danger text-sm text-center p-2 bg-red-500/10 rounded">
              {error}
            </div>
          )}

          <button
            className="w-full px-4 py-2.5 rounded-lg bg-button-primary text-white font-semibold text-sm transition-colors cursor-pointer border-none hover:bg-button-hover focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2 disabled:opacity-60 disabled:cursor-not-allowed"
            type="submit"
          >
            {isRegistering ? "Sign Up" : "Login"}
          </button>
        </form>

        <div className="mt-6 text-center text-sm text-text-secondary">
          <button
            className="bg-transparent border-none text-link cursor-pointer px-1 font-medium hover:underline focus-visible:outline-2 focus-visible:outline-link focus-visible:outline-offset-2 rounded-sm"
            onClick={() => setIsRegistering(!isRegistering)}
          >
            {isRegistering
              ? "Already have an account? Login"
              : "Need an account? Register"}
          </button>
        </div>
      </div>
    </div>
  );
}
