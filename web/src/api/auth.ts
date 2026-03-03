import { account, ID } from "@/lib/appwrite";

export async function getCurrentUser() {
  try {
    return await account.get();
  } catch {
    return null;
  }
}

export async function loginWithEmail(email: string, password: string) {
  return await account.createEmailPasswordSession({
    email: email,
    password: password,
  });
}

export async function registerWithEmail(
  email: string,
  password: string,
  name: string,
) {
  await account.create({userId: ID.unique(),email: email,password: password,name: name});
  return await loginWithEmail(email, password);
}

export async function logoutUser() {
  return await account.deleteSession({ sessionId: "current" });
}
