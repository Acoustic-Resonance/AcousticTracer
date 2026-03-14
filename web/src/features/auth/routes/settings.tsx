import { useState, useEffect } from "react";
import { useNavigate } from "react-router";
import { useUser } from "../context/user-store";
import { account } from "@/lib/appwrite";
import { getErrorMessage } from "@/utils/get-error-message";
import { showToast } from "@/components/toast";
import {
  User,
  Mail,
  Lock,
  AlertTriangle,
  LogOut,
  Shield,
  Info,
  ExternalLink,
  Trash2,
} from "lucide-react";
import ChangePasswordForm from "../components/change-password-form";
import AppSidebar from "@/components/app-layout";

export default function Settings() {
  const { current, logout, refreshUser } = useUser();
  const navigate = useNavigate();

  const [name, setName] = useState(current?.name ?? "");
  const [email, setEmail] = useState(current?.email ?? "");
  const [passwordForEmail, setPasswordForEmail] = useState("");
  const [profileError, setProfileError] = useState("");
  const [isUpdatingName, setIsUpdatingName] = useState(false);
  const [isUpdatingEmail, setIsUpdatingEmail] = useState(false);
  const [showDeleteConfirm, setShowDeleteConfirm] = useState(false);
  const [isDeleting, setIsDeleting] = useState(false);

  // Keep local state in sync if user data updates
  useEffect(() => {
    if (current) {
      setName(current.name);
      setEmail(current.email);
    }
  }, [current]);

  const handleUpdateName = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!name.trim()) return;
    setProfileError("");
    setIsUpdatingName(true);
    try {
      await account.updateName({ name: name.trim() });
      showToast("Name updated successfully!", "success");
      await refreshUser();
    } catch (err: unknown) {
      setProfileError(getErrorMessage(err));
    } finally {
      setIsUpdatingName(false);
    }
  };

  const handleUpdateEmail = async (e: React.FormEvent) => {
    e.preventDefault();
    setProfileError("");
    if (!email.trim()) {
      setProfileError("Email cannot be empty.");
      return;
    }
    if (!passwordForEmail) {
      setProfileError("Password is required to update email.");
      return;
    }
    setIsUpdatingEmail(true);
    try {
      await account.updateEmail({
        email: email.trim(),
        password: passwordForEmail,
      });
      showToast("Email updated successfully!", "success");
      setPasswordForEmail("");
      await refreshUser();
    } catch (err: unknown) {
      setProfileError(getErrorMessage(err));
    } finally {
      setIsUpdatingEmail(false);
    }
  };

  const handleLogout = async () => {
    await logout();
    navigate("/auth/login");
  };

  // OAuth-only users have no password set — passwordUpdate is "" or a very old default
  const isOAuthOnly = !current?.passwordUpdate;

  const initials = current?.name
    ? current.name
        .split(" ")
        .map((w) => w[0])
        .join("")
        .toUpperCase()
        .slice(0, 2)
    : "?";

  return (
    <AppSidebar>
      <div className="flex-1 overflow-y-auto">
        <div className="max-w-2xl mx-auto px-6 py-10 sm:px-8">
          <div className="mb-10">
            <h1 className="text-2xl font-bold text-text-primary mb-6">
              Settings
            </h1>
            <div className="relative overflow-hidden rounded-2xl border border-border-primary bg-bg-card">
              <div className="absolute inset-0 bg-linear-to-r from-button-primary/8 via-transparent to-accent/8" />
              <div className="relative flex items-center gap-5 p-6">
                <div className="h-14 w-14 rounded-full bg-accent flex items-center justify-center text-bg-primary text-lg font-bold ring-2 ring-white/10 shadow-md">
                  {initials}
                </div>
                <div className="flex-1 min-w-0">
                  <p className="text-lg font-semibold text-text-primary truncate">
                    {current?.name ?? "User"}
                  </p>
                  <p className="text-sm text-text-secondary truncate">
                    {current?.email ?? "—"}
                  </p>
                </div>
                <div className="hidden sm:flex flex-col items-end gap-1 text-xs text-text-secondary">
                  <span className="px-2.5 py-1 rounded-full bg-accent/10 text-accent font-medium">
                    Active
                  </span>
                </div>
              </div>
            </div>
          </div>

          {profileError && (
            <div className="flex items-center gap-2 text-danger text-sm p-3 bg-red-500/10 border border-red-500/20 rounded-xl mb-6">
              <AlertTriangle className="h-4 w-4 shrink-0" />
              {profileError}
            </div>
          )}

          <section className="group bg-bg-card rounded-2xl border border-border-primary p-6 mb-4 transition-colors hover:border-border-primary/80">
            <div className="flex items-center gap-3 mb-5">
              <div className="h-9 w-9 rounded-xl bg-button-primary/10 flex items-center justify-center">
                <User className="h-4.5 w-4.5 text-button-primary" />
              </div>
              <div>
                <h2 className="text-sm font-semibold text-text-primary">
                  Display Name
                </h2>
                <p className="text-xs text-text-secondary">
                  How others see you in the app
                </p>
              </div>
            </div>
            <form onSubmit={handleUpdateName} className="flex gap-3">
              <input
                id="settings-name"
                type="text"
                value={name}
                onChange={(e) => setName(e.target.value)}
                placeholder={"Display Name"}
                required
                className="flex-1 py-2.5 px-3.5 border border-border-primary bg-input-bg text-text-primary rounded-xl text-sm focus:outline-none focus:border-button-primary focus:ring-1 focus:ring-button-primary/30 transition-all"
              />
              <button
                type="submit"
                disabled={isUpdatingName || name === current?.name}
                className="px-5 py-2.5 rounded-xl bg-button-primary text-white text-sm font-semibold hover:bg-button-hover disabled:opacity-50 disabled:cursor-not-allowed transition-all cursor-pointer"
              >
                {isUpdatingName ? "Saving..." : "Save"}
              </button>
            </form>
          </section>

          <section className="group bg-bg-card rounded-2xl border border-border-primary p-6 mb-4 transition-colors hover:border-border-primary/80">
            <div className="flex items-center gap-3 mb-5">
              <div className="h-9 w-9 rounded-xl bg-blue-500/10 flex items-center justify-center">
                <Mail className="h-4.5 w-4.5 text-blue-400" />
              </div>
              <div>
                <h2 className="text-sm font-semibold text-text-primary">
                  Email Address
                </h2>
                <p className="text-xs text-text-secondary">
                  Used for login and notifications
                </p>
              </div>
            </div>
            {isOAuthOnly ? (
              <a
                href="https://myaccount.google.com/email"
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-start gap-3 p-4 rounded-xl bg-blue-500/5 border border-blue-500/15 hover:bg-blue-500/10 hover:border-blue-500/25 transition-all cursor-pointer no-underline group/link"
              >
                <Info className="h-4 w-4 text-blue-400 mt-0.5 shrink-0" />
                <div className="flex-1">
                  <p className="text-sm text-text-primary">
                    Your email is managed by Google
                  </p>
                  <p className="text-xs text-text-secondary mt-1">
                    Since you signed in with Google, your email (
                    {current?.email}) is linked to your Google account. Click
                    here to manage it.
                  </p>
                </div>
                <ExternalLink className="h-4 w-4 text-blue-400/60 group-hover/link:text-blue-400 mt-0.5 shrink-0 transition-colors" />
              </a>
            ) : (
              <form onSubmit={handleUpdateEmail} className="space-y-3">
                <input
                  id="settings-email"
                  type="email"
                  onChange={(e) => setEmail(e.target.value)}
                  placeholder="Enter new email"
                  className="w-full py-2.5 px-3.5 border border-border-primary bg-input-bg text-text-primary rounded-xl text-sm focus:outline-none focus:border-button-primary focus:ring-1 focus:ring-button-primary/30 transition-all"
                />
                <div className="flex gap-3">
                  <input
                    id="settings-email-password"
                    type="password"
                    value={passwordForEmail}
                    onChange={(e) => setPasswordForEmail(e.target.value)}
                    placeholder="Current password to confirm"
                    className="flex-1 py-2.5 px-3.5 border border-border-primary bg-input-bg text-text-primary rounded-xl text-sm focus:outline-none focus:border-button-primary focus:ring-1 focus:ring-button-primary/30 transition-all placeholder:text-text-secondary/50"
                  />
                  <button
                    type="submit"
                    disabled={
                      isUpdatingEmail ||
                      email === current?.email ||
                      !passwordForEmail
                    }
                    title={
                      email === current?.email
                        ? "Please change email"
                        : !passwordForEmail
                          ? "Enter your current password"
                          : undefined
                    }
                    className="px-5 py-2.5 rounded-xl bg-button-primary text-white text-sm font-semibold hover:bg-button-hover disabled:opacity-50 disabled:cursor-not-allowed transition-all cursor-pointer"
                  >
                    {isUpdatingEmail ? "Saving..." : "Save"}
                  </button>
                </div>
              </form>
            )}
          </section>

          <section className="group bg-bg-card rounded-2xl border border-border-primary p-6 mb-4 transition-colors hover:border-border-primary/80">
            <div className="flex items-center gap-3 mb-5">
              <div className="h-9 w-9 rounded-xl bg-amber-500/10 flex items-center justify-center">
                <Lock className="h-4.5 w-4.5 text-amber-400" />
              </div>
              <div>
                <h2 className="text-sm font-semibold text-text-primary">
                  Security
                </h2>
                <p className="text-xs text-text-secondary">
                  Update your password
                </p>
              </div>
            </div>
            {isOAuthOnly ? (
              <a
                href="https://myaccount.google.com/security"
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-start gap-3 p-4 rounded-xl bg-amber-500/5 border border-amber-500/15 hover:bg-amber-500/10 hover:border-amber-500/25 transition-all cursor-pointer no-underline group/link"
              >
                <Info className="h-4 w-4 text-amber-400 mt-0.5 shrink-0" />
                <div className="flex-1">
                  <p className="text-sm text-text-primary">
                    Password managed by Google
                  </p>
                  <p className="text-xs text-text-secondary mt-1">
                    You signed in with Google and don't have a separate
                    password. Click here to manage your Google security
                    settings.
                  </p>
                </div>
                <ExternalLink className="h-4 w-4 text-amber-400/60 group-hover/link:text-amber-400 mt-0.5 shrink-0 transition-colors" />
              </a>
            ) : (
              <ChangePasswordForm />
            )}
          </section>

          <section className="bg-bg-card rounded-2xl border border-danger/20 p-6 mt-8">
            <div className="flex items-center gap-3 mb-4">
              <div className="h-9 w-9 rounded-xl bg-danger/10 flex items-center justify-center">
                <Shield className="h-4.5 w-4.5 text-danger" />
              </div>
              <div>
                <h2 className="text-sm font-semibold text-danger">
                  Danger Zone
                </h2>
                <p className="text-xs text-text-secondary">
                  Irreversible actions
                </p>
              </div>
            </div>
            <div className="flex items-center justify-between p-4 rounded-xl bg-danger/5 border border-danger/10">
              <div>
                <p className="text-sm font-medium text-text-primary">
                  Log out everywhere
                </p>
                <p className="text-xs text-text-secondary mt-0.5">
                  End all active sessions on every device
                </p>
              </div>
              <button
                onClick={handleLogout}
                className="flex items-center gap-2 px-4 py-2 rounded-xl border border-danger/40 text-danger text-sm font-semibold hover:bg-danger/10 transition-all cursor-pointer bg-transparent"
              >
                <LogOut className="h-3.5 w-3.5" />
                Log Out
              </button>
            </div>
            <div className="flex items-center justify-between p-4 rounded-xl bg-danger/5 border border-danger/10 mt-3">
              <div>
                <p className="text-sm font-medium text-text-primary">
                  Delete account
                </p>
                <p className="text-xs text-text-secondary mt-0.5">
                  Permanently delete your account and all data
                </p>
              </div>
              <button
                onClick={() => setShowDeleteConfirm(true)}
                className="flex items-center gap-2 px-4 py-2 rounded-xl bg-danger text-white text-sm font-semibold hover:bg-danger/80 transition-all cursor-pointer border-none"
              >
                <Trash2 className="h-3.5 w-3.5" />
                Delete
              </button>
            </div>
          </section>

          {showDeleteConfirm && (
            <div className="fixed inset-0 flex items-center justify-center backdrop-blur-sm z-50 bg-black/60">
              <div className="bg-bg-card rounded-2xl p-6 shadow-lg border border-border-primary w-96 mx-4">
                <div className="flex items-center gap-3 mb-4">
                  <div className="h-10 w-10 rounded-xl bg-danger/10 flex items-center justify-center">
                    <Trash2 className="h-5 w-5 text-danger" />
                  </div>
                  <h3 className="text-lg font-semibold text-text-primary">
                    Delete Account
                  </h3>
                </div>
                <p className="text-sm text-text-secondary mb-6 leading-relaxed">
                  This will permanently delete your account and all associated
                  data. This action{" "}
                  <strong className="text-text-primary">
                    cannot be undone
                  </strong>
                  .
                </p>
                <div className="flex justify-end gap-3">
                  <button
                    onClick={() => setShowDeleteConfirm(false)}
                    disabled={isDeleting}
                    className="px-4 py-2 rounded-xl border border-border-primary text-text-primary text-sm font-medium hover:bg-white/5 transition-all cursor-pointer bg-transparent disabled:opacity-50"
                  >
                    Cancel
                  </button>
                  <button
                    onClick={async () => {
                      setIsDeleting(true);
                      try {
                        await account.updateStatus();
                        showToast("Account deleted.", "success");
                        await logout();
                        navigate("/auth/login");
                      } catch (err: unknown) {
                        showToast(getErrorMessage(err), "error");
                        setIsDeleting(false);
                      }
                    }}
                    disabled={isDeleting}
                    className="flex items-center gap-2 px-4 py-2 rounded-xl bg-danger text-white text-sm font-semibold hover:bg-danger/80 transition-all cursor-pointer border-none disabled:opacity-60 disabled:cursor-not-allowed"
                  >
                    <Trash2 className="h-3.5 w-3.5" />
                    {isDeleting ? "Deleting..." : "Delete Account"}
                  </button>
                </div>
              </div>
            </div>
          )}

          {/* Bottom spacer */}
          <div className="h-10" />
        </div>
      </div>
    </AppSidebar>
  );
}
