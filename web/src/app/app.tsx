import { AppRouter } from "./router";
import { AppProvider } from "./provider";
export default function App() {
  return (
    <AppProvider>
      <AppRouter />
    </AppProvider>
  );
}
