import { useState } from "react";
import { Link, useNavigate } from "react-router";
import { useUser } from "../context/user-store";
import { getErrorMessage } from "@/utils/get-error-message";
import { showToast } from "@/components/toast";
import { Waves } from "lucide-react";
import GoogleButton from "@/components/ui/google-auth";

export default function Login() {
  const navigate = useNavigate();
  const { login } = useUser();
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError("");
    setIsSubmitting(true);
    try {
      await login(email, password);
      showToast("Login successful — welcome back!", "success");
      navigate("/");
    } catch (err: unknown) {
      setError(getErrorMessage(err));
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="flex items-center justify-center min-h-screen bg-bg-primary p-4">
      <div className="bg-bg-card rounded-xl shadow-md max-w-md w-full p-10">
        {/* Logo */}
        <div className="flex justify-center mb-6">
          <div className="flex h-12 w-12 items-center justify-center rounded-2xl bg-accent/10">
            <Waves className="h-6 w-6 text-accent" />
          </div>
        </div>

        <h1 className="text-center mb-2 text-2xl font-bold text-text-primary">
          Welcome Back
        </h1>
        <p className="text-center mb-8 text-sm text-text-secondary">
          Sign in to your Acoustic Tracer account
        </p>

        <form onSubmit={handleSubmit} className="flex flex-col gap-5">
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
              {error.includes("already logged in") && (
                <>
                  {" "}
                  <Link
                    to="/"
                    className="text-link font-medium hover:underline no-underline"
                  >
                    home page
                  </Link>
                  .{"\n"}
                </>
              )}
            </div>
          )}

          <div className="flex justify-end">
            <Link
              to="/auth/forgot-password"
              className="text-link text-xs hover:underline no-underline"
            >
              Forgot your password?
            </Link>
          </div>

          <button
            className="w-full px-4 py-2.5 rounded-lg bg-button-primary text-white font-semibold text-sm transition-colors cursor-pointer border-none hover:bg-button-hover focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2 disabled:opacity-60 disabled:cursor-not-allowed"
            type="submit"
            disabled={isSubmitting}
          >
            {isSubmitting ? "Signing in..." : "Login"}
          </button>
        </form>
        <GoogleButton />
        <div className="mt-6 text-center text-sm text-text-secondary">
          Don't have an account?{" "}
          <Link
            to="/auth/register"
            className="text-link font-medium hover:underline no-underline"
          >
            Create one
          </Link>
        </div>
      </div>
    </div>
  );
}
