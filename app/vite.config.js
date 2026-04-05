import { defineConfig } from "vite";

export default defineConfig({
  server: {
    proxy: {
      "/api": process.env.VSHOGI_BACKEND_ORIGIN ?? "http://127.0.0.1:8080",
    },
  },
});
