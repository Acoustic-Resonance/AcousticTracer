# AcousticTracer Web (Vite + React, JavaScript)

This is a skeleton frontend for the pipeline described in `docs/projectsummary.md`.

Most implementation code has been intentionally replaced with `TODO` stubs so you can write it yourself.

## Requirements

- Node.js 18+ recommended
- npm 9+ recommended

## Install + run

```bash
cd web
npm install
npm run dev
```

Open the URL Vite prints (usually `http://localhost:5173`).

## Dev API proxy

During development, requests to `/api/*` are proxied to a backend target:

- Default: `http://localhost:8080`
- Override: set `VITE_API_PROXY_TARGET`

Example:

```bash
VITE_API_PROXY_TARGET=http://localhost:3000 npm run dev
```

## Expected backend endpoints (MVP)

- `POST /api/simulations` (multipart)
  - form field `model`: `.glb/.gltf` file
  - form field `params`: JSON string
  - response: `{ "id": "..." }`

- `GET /api/simulations/:id/status`
  - response: `{ "status": "queued"|"running"|"done"|"error", "progress"?: 0..1, "error"?: string }`

- `GET /api/simulations/:id/meta` (later)

- `GET /api/simulations/:id/chunks/:index` (later)

## Where to extend next

- Implement the UI flow in [web/src/App.jsx](web/src/App.jsx) and components in [web/src/components](web/src/components)
- Implement HTTP + backend API calls in [web/src/api](web/src/api)
- Implement R3F renderer placeholders in [web/src/r3f](web/src/r3f)
- Implement playback/chunk decoding placeholders in [web/src/playback](web/src/playback)
