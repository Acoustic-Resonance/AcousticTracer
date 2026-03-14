import { useState } from "react";
import { Link, useNavigate } from "react-router";
import { useUser } from "../context/user-store";
import { getErrorMessage } from "@/utils/get-error-message";
import { showToast } from "@/components/toast";
import { Waves } from "lucide-react";
import GoogleButton from "@/components/ui/google-auth";

export default function Register() {
  const navigate = useNavigate();
  const { register } = useUser();
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    setError("");

    if (!name.trim()) {
      setError("Please enter your name");
      return;
    }
    if (!email.trim()) {
      setError("Please enter your email");
      return;
    }
    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
      setError("Please enter a valid email address");
      return;
    }
    if (password.length < 8) {
      setError("Password must be at least 8 characters long");
      return;
    }

    setIsSubmitting(true);
    try {
      await register(email, password, name);
      showToast("Registration successful — welcome!", "success");
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
          Create Account
        </h1>
        <p className="text-center mb-8 text-sm text-text-secondary">
          Get started with Acoustic Tracer
        </p>

        <form
          onSubmit={handleSubmit}
          autoComplete="off"
          className="flex flex-col gap-5"
        >
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
              autoComplete="name"
              placeholder="Enter your name"
              required
              value={name}
              onChange={(e) => setName(e.target.value)}
            />
          </div>

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
              type="text"
              autoComplete="off"
              placeholder="Enter your email"
              required
              value={email}
              onChange={(e) => setEmail(e.target.value)}
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
              autoComplete="new-password"
              placeholder="Enter your password"
              required
              value={password}
              onChange={(e) => setPassword(e.target.value)}
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
            disabled={isSubmitting}
          >
            {isSubmitting ? "Registering..." : "Sign Up"}
          </button>
        </form>
        <GoogleButton />

        <div className="mt-6 text-center text-sm text-text-secondary">
          Already have an account?{" "}
          <Link
            to="/auth/login"
            className="text-link font-medium hover:underline no-underline"
          >
            Log in
          </Link>
        </div>
      </div>
    </div>
  );
}
