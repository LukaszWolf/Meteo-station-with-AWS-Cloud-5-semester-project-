import react from '@vitejs/plugin-react';
import {defineConfig} from 'vite';

export default defineConfig({
  plugins: [react()],
  base: process.env.VITE_BASE_PATH || '/meteo-app/',
  define: {
    global: 'globalThis',  // naprawia błąd "global is not defined"
  },
});
