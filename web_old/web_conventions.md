# Web Conventions (Vite + React)

This document defines conventions for the `web/` folder.
It is meant to complement:

- `docs/communication_standards.md` (data contract and payload shapes)
- `docs/workflow.md` (branching/PR workflow)

> Note: This is a frontend skeleton; many modules intentionally contain `TODO` stubs.

---

## Goals

- Keep UI code readable and predictable.
- Keep backend contract logic centralized.
- Make it hard to accidentally drift from the agreed API payload shapes.
- Make changes easy to review in PRs.

---

## Folder Responsibilities

- `src/api/`
  - HTTP utilities and backend client functions.
  - **All** `/api/*` calls go through here (no `fetch` inside UI components).

- `src/components/`
  - Presentational and UI orchestration components.
  - Components should not know URL paths or perform raw network calls.

- `src/state/`
  - App-level state machine / reducer / state helpers.
  - Keeps `App.jsx` thin and prevents "state spaghetti".

- `src/playback/`
  - Playback controller, chunk caching, and binary decoding.
  - Keep pure decoding logic separate from network/cache logic.

- `src/r3f/`
  - Rendering code using React Three Fiber.
  - Renders data produced by playback/controller layers.

- `src/utils/`
  - Small pure helpers (time conversions, voxel indexing, etc.).

- `src/types/`
  - JSDoc typedefs / documentation-only type contracts (since this project uses JS).

---

## Naming

- File names: `camelCase.js` / `PascalCase.jsx` depending on existing folder convention.
  - Components: `PascalCase.jsx` (React components)
  - Modules/utilities: `camelCase.js`

- Exported functions: `camelCase`.
- React components: `PascalCase`.

---

## API Contract Boundary

The API contract described in `docs/communication_standards.md` uses `snake_case` JSON keys.

Rule:

- Use `snake_case` end-to-end in the frontend and backend.
- Client code in `src/api/*` should send/receive `snake_case` payloads without key conversion.

Why:

- Eliminates confusion about key naming and avoids conversion bugs.
- Keeps docs, backend, and frontend aligned on a single canonical shape.

TODO:

- Freeze the canonical shapes for:
  - `POST /api/simulations` params
  - `GET /api/simulations/:id/status` response
  - `meta.json` response

---

## Error Handling

- Network and parsing errors should be normalized in `src/api/http.js`.
- UI components should render user-friendly messages and avoid leaking raw stack traces.

TODO:

- Decide a standard error shape for API failures (status code + message + optional details).

---

## Formatting

We use **Prettier** to keep formatting consistent across the team.

- Format the web codebase: `npm run format`
- Check formatting in CI/PRs: `npm run format:check`

Why:

- Reduces style nitpicks in PR reviews.
- Reduces merge conflicts caused by inconsistent formatting.

---

## TODO (Later): ESLint

Add ESLint once there is more implementation code and we want automated code-quality checks (unused vars, React hooks rules, etc.).
