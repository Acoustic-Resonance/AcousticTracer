/**
 * Safely extract an error message from an unknown thrown value.
 *
 * Handles `Error` instances, Appwrite error objects with a `message`
 * property, and arbitrary values by falling back to `String()`.
 */
export function getErrorMessage(err: unknown): string {
  if (err instanceof Error) return err.message;
  if (
    typeof err === "object" &&
    err !== null &&
    "message" in err &&
    typeof (err as { message: unknown }).message === "string"
  ) {
    return (err as { message: string }).message;
  }
  return String(err);
}
