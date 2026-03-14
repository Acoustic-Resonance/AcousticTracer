import { useState } from "react";
import { useSearchParams, Link } from "react-router";
import { account } from "@/lib/appwrite";
import { getErrorMessage } from "@/utils/get-error-message";
import { showToast } from "@/components/toast";
import { Waves, ArrowLeft, CheckCircle, AlertTriangle } from "lucide-react";

export default function ResetPassword() {
  const [searchParams] = useSearchParams();
  const userId = searchParams.get("userId") ?? "";
  const secret = searchParams.get("secret") ?? "";

  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [error, setError] = useState("");
  const [success, setSuccess] = useState(false);
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    setError("");

    if (password.length < 8) {
      setError("Password must be at least 8 characters.");
      return;
    }
    if (password !== confirmPassword) {
      setError("Passwords do not match.");
      return;
    }

    setIsSubmitting(true);
    try {
      await account.updateRecovery({
        userId,
        secret,
        password,
      });
      setSuccess(true);
      showToast("Password reset successfully!", "success");
    } catch (err: unknown) {
      setError(getErrorMessage(err));
    } finally {
      setIsSubmitting(false);
    }
  };

  // Missing or invalid token
  if (!userId || !secret) {
    return (
      <div className="flex items-center justify-center min-h-screen bg-bg-primary p-4">
        <div className="bg-bg-card rounded-xl shadow-md max-w-md w-full p-10 text-center">
          <div className="flex justify-center mb-4">
            <div className="flex h-14 w-14 items-center justify-center rounded-full bg-red-500/10">
              <AlertTriangle className="h-7 w-7 text-danger" />
            </div>
          </div>
          <h1 className="mb-2 text-2xl font-bold text-text-primary">
            Invalid Reset Link
          </h1>
          <p className="text-sm text-text-secondary mb-6">
            This password reset link is invalid or has expired. Please request a
            new one.
          </p>
          <Link
            to="/auth/forgot-password"
            className="inline-block px-6 py-2.5 bg-button-primary text-white rounded-lg no-underline text-sm font-semibold hover:bg-button-hover transition-colors"
          >
            Request New Link
          </Link>
        </div>
      </div>
    );
  }

  // Success state
  if (success) {
    return (
      <div className="flex items-center justify-center min-h-screen bg-bg-primary p-4">
        <div className="bg-bg-card rounded-xl shadow-md max-w-md w-full p-10 text-center">
          <div className="flex justify-center mb-4">
            <div className="flex h-14 w-14 items-center justify-center rounded-full bg-green-500/10">
              <CheckCircle className="h-7 w-7 text-green-400" />
            </div>
          </div>
          <h1 className="mb-2 text-2xl font-bold text-text-primary">
            Password Reset!
          </h1>
          <p className="text-sm text-text-secondary mb-6">
            Your password has been updated successfully. You can now log in with
            your new password.
          </p>
          <Link
            to="/auth/login"
            className="inline-block px-6 py-2.5 bg-button-primary text-white rounded-lg no-underline text-sm font-semibold hover:bg-button-hover transition-colors"
          >
            Go to Login
          </Link>
        </div>
      </div>
    );
  }

  // Reset form
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
          Reset Password
        </h1>
        <p className="text-center mb-8 text-sm text-text-secondary">
          Choose a new password for your account
        </p>

        <form onSubmit={handleSubmit} className="flex flex-col gap-5">
          <div className="flex flex-col gap-2">
            <label
              htmlFor="password"
              className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
            >
              New Password
            </label>
            <input
              id="password"
              className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
              type="password"
              placeholder="Enter new password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              autoComplete="new-password"
              autoFocus
            />
            <p className="text-xs text-text-secondary">
              Must be at least 8 characters
            </p>
          </div>

          <div className="flex flex-col gap-2">
            <label
              htmlFor="confirm-password"
              className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
            >
              Confirm Password
            </label>
            <input
              id="confirm-password"
              className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
              type="password"
              placeholder="Confirm new password"
              value={confirmPassword}
              onChange={(e) => setConfirmPassword(e.target.value)}
              autoComplete="new-password"
            />
          </div>

          {error && (
            <div className="text-danger text-sm text-center p-2 bg-red-500/10 rounded">
              {error}
            </div>
          )}

          <button
            type="submit"
            disabled={isSubmitting}
            className="w-full px-4 py-2.5 rounded-lg bg-button-primary text-white font-semibold text-sm transition-colors cursor-pointer border-none hover:bg-button-hover focus-visible:outline-2 focus-visible:outline-button-primary focus-visible:outline-offset-2 disabled:opacity-60 disabled:cursor-not-allowed"
          >
            {isSubmitting ? "Resetting..." : "Reset Password"}
          </button>
        </form>

        <div className="mt-6 text-center">
          <Link
            to="/auth/login"
            className="inline-flex items-center gap-2 text-sm text-link font-medium hover:underline no-underline"
          >
            <ArrowLeft className="h-4 w-4" />
            Back to Login
          </Link>
        </div>
      </div>
    </div>
  );
}
