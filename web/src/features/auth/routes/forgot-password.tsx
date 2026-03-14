import { useState } from "react";
import { Link } from "react-router";
import { account } from "@/lib/appwrite";
import { getErrorMessage } from "@/utils/get-error-message";
import { showToast } from "@/components/toast";
import { Waves, ArrowLeft, Mail } from "lucide-react";

export default function ForgotPassword() {
  const [email, setEmail] = useState("");
  const [error, setError] = useState("");
  const [sent, setSent] = useState(false);
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError("");

    if (!email.trim()) {
      setError("Please enter your email address.");
      return;
    }

    if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)) {
      setError("Please enter a valid email address.");
      return;
    }

    setIsSubmitting(true);
    try {
      await account.createRecovery({
        email,
        url: `${window.location.origin}/auth/reset-password`,
      });
      setSent(true);
      showToast("Recovery email sent! Check your inbox.", "success");
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

        {sent ? (
          <div className="text-center">
            <div className="flex justify-center mb-4">
              <div className="flex h-14 w-14 items-center justify-center rounded-full bg-green-500/10">
                <Mail className="h-7 w-7 text-green-400" />
              </div>
            </div>
            <h1 className="mb-2 text-2xl font-bold text-text-primary">
              Check Your Email
            </h1>
            <p className="mb-6 text-sm text-text-secondary leading-relaxed">
              We sent a password reset link to{" "}
              <span className="font-medium text-text-primary">{email}</span>.
              <br />
              Click the link in the email to reset your password.
            </p>
            <p className="text-xs text-text-secondary mb-4">
              Didn't receive the email? Check your spam folder or{" "}
              <button
                type="button"
                onClick={() => setSent(false)}
                className="text-link font-medium hover:underline bg-transparent border-none cursor-pointer p-0"
              >
                try again
              </button>
              .
            </p>
            <p className="text-xs text-text-secondary/70 mb-6">
              Signed in with Google? This will create a password for email
              login.
            </p>
            <Link
              to="/auth/login"
              className="inline-flex items-center gap-2 text-sm text-link font-medium hover:underline no-underline"
            >
              <ArrowLeft className="h-4 w-4" />
              Back to Login
            </Link>
          </div>
        ) : (
          <>
            <h1 className="text-center mb-2 text-2xl font-bold text-text-primary">
              Forgot Password?
            </h1>
            <p className="text-center mb-8 text-sm text-text-secondary">
              Enter your email and we'll send you a reset link
            </p>

            <form onSubmit={handleSubmit} className="flex flex-col gap-5">
              <div className="flex flex-col gap-2">
                <label
                  htmlFor="email"
                  className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
                >
                  Email Address
                </label>
                <input
                  id="email"
                  className="w-full py-2.5 px-3 border border-border-primary bg-input-bg text-text-primary rounded-lg text-sm transition-colors focus:outline-none focus:border-button-primary"
                  type="email"
                  placeholder="Enter your email"
                  value={email}
                  onChange={(e) => setEmail(e.target.value)}
                  autoFocus
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
                {isSubmitting ? "Sending..." : "Send Reset Link"}
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
          </>
        )}
      </div>
    </div>
  );
}
