import { useState } from "react";
import { account } from "@/lib/appwrite";
import { getErrorMessage } from "@/utils/get-error-message";
import { showToast } from "@/components/toast";

export default function ChangePasswordForm() {
  const [currentPassword, setCurrentPassword] = useState("");
  const [newPassword, setNewPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [error, setError] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setError("");

    if (newPassword.length < 8) {
      setError("New password must be at least 8 characters.");
      return;
    }

    if (newPassword !== confirmPassword) {
      setError("Passwords do not match.");
      return;
    }

    setIsSubmitting(true);
    try {
      await account.updatePassword({
        password: newPassword,
        oldPassword: currentPassword,
      });
      showToast("Password updated successfully!", "success");
      setCurrentPassword("");
      setNewPassword("");
      setConfirmPassword("");
    } catch (err: unknown) {
      setError(getErrorMessage(err));
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      {error && (
        <div className="text-danger text-sm p-3 bg-red-500/10 border border-red-500/20 rounded-xl">
          {error}
        </div>
      )}

      <div>
        <label
          htmlFor="current-password"
          className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
        >
          Current Password
        </label>
        <input
          id="current-password"
          type="password"
          value={currentPassword}
          onChange={(e) => setCurrentPassword(e.target.value)}
          className="w-full py-2.5 px-3.5 border border-border-primary bg-input-bg text-text-primary rounded-xl text-sm focus:outline-none focus:border-button-primary focus:ring-1 focus:ring-button-primary/30 transition-all"
          autoComplete="current-password"
        />
      </div>

      <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
        <div>
          <label
            htmlFor="new-password"
            className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
          >
            New Password
          </label>
          <input
            id="new-password"
            type="password"
            value={newPassword}
            onChange={(e) => setNewPassword(e.target.value)}
            className="w-full py-2.5 px-3.5 border border-border-primary bg-input-bg text-text-primary rounded-xl text-sm focus:outline-none focus:border-button-primary focus:ring-1 focus:ring-button-primary/30 transition-all"
            autoComplete="new-password"
          />
        </div>

        <div>
          <label
            htmlFor="confirm-password"
            className="block text-xs font-semibold text-text-secondary mb-1.5 uppercase tracking-wide"
          >
            Confirm New Password
          </label>
          <input
            id="confirm-password"
            type="password"
            value={confirmPassword}
            onChange={(e) => setConfirmPassword(e.target.value)}
            className="w-full py-2.5 px-3.5 border border-border-primary bg-input-bg text-text-primary rounded-xl text-sm focus:outline-none focus:border-button-primary focus:ring-1 focus:ring-button-primary/30 transition-all"
            autoComplete="new-password"
          />
        </div>
      </div>

      <button
        type="submit"
        disabled={
          isSubmitting || !currentPassword || !newPassword || !confirmPassword
        }
        className="px-5 py-2.5 rounded-xl bg-button-primary text-white text-sm font-semibold hover:bg-button-hover disabled:opacity-50 disabled:cursor-not-allowed transition-all cursor-pointer"
      >
        {isSubmitting ? "Updating..." : "Update Password"}
      </button>
    </form>
  );
}
