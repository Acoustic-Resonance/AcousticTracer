# AcousticTracer — The Development Story

> A narrative history of how four developers built an acoustic ray-tracing
> simulation from a blank proposal to a full-stack 3D visualisation app in under
> two months.

---

## Cast of Characters

| Developer | GitHub Handle | Primary Domain |
|-----------|---------------|----------------|
| **Alex Wright** | `alexodwright` | Core C engine, networking, 3D scene integration |
| **Patryk Mrozek** | `patrykmrozek` | Ray tracing, voxel grid, simulation loop |
| **Eoghan Murphy** | `EonMurph` | BVH acceleration, model loading, documentation |
| **Michael McCarthy** | `mccarthy-michael` | Web frontend, UI/UX, state management |

Over the course of **307 commits** (including 85 merged pull requests) from
**January 14 to March 8, 2026**, the team transformed a LaTeX proposal into a
working acoustic simulation with a React-based 3D visualiser.

---

## Prologue — The Idea (January 14)

It all started with a single file: `proposal.tex`.

Alex pushed the **initial project proposal draft** — a LaTeX document sketching
the vision: *simulate how sound waves travel through a 3D room, bouncing off
walls and ceilings, and visualise the results*. Think ray tracing, but for
sound instead of light.

Within hours on that same day, the team assembled. Patryk, Alex, Eoghan, and
Michael each created their contributor files in the `contributors/` directory —
a simple ritual that marked the beginning of a shared endeavour. The proposal
rapidly evolved as Alex added desired features, environment construction specs,
and a tech stack, while Eoghan reformatted the document and fixed typos.

> **PR #1** — the very first pull request — merged the proposal into `main`.

Michael set up the team's **workflow document** (`docs/workflow.md`), defining a
fork-based contribution model with draft PRs, naming conventions
(`feature/`, `fix/`, `docs/`), and a sync-before-branching discipline. This
workflow would carry the team through the entire project.

**Commit count this day: 18** — the highest single-day count in the project's
early life.

---

## Chapter 1 — Laying the Foundation (January 15 – 22)

### Scaffolding the C Core

On **January 19**, Alex committed the first real code: `add core and cmake
(cross-platform?)` — a tentative commit message that hinted at the uncertainty
of bootstrapping a cross-platform C project. Patryk immediately followed with a
CMake fix (`fix cmake..`), establishing a pattern that would repeat throughout
the project: one person lays the groundwork, another quickly patches the rough
edges.

The team established C coding conventions in `docs/c_conventions.md` — `AT_`
prefixed types in PascalCase, snake_case for functions and variables, Javadoc
comments. Alex defined the **public API** in `at.h` and the math primitives in
`at_math.h`. These header files became the contracts that the rest of the engine
would be built against.

### Defining the Protocol

Eoghan authored `communication_standards.md`, defining the JSON data format for
how the frontend would talk to the simulation engine — specifying fields like
`glb_path`, `voxel_size`, `materials`, `fps`, `num_rays`, and source
information. This early specification document would prove prescient: the team
largely stuck to this contract as the project grew.

### The First 3D Assets

Eoghan created the first room models — simple **cube room** files in
`assets/glb/` — and added them to the project. These would serve as test
environments for every stage of the simulation engine.

### The Web Skeleton

On **January 22**, Michael laid down the web application scaffolding: `Add web
folder with a detailed file structure containing stub files and TODO's`. Every
file was a placeholder, every function a promise. But the structure was
thoughtful and intentional — it would guide the frontend development that
followed.

**Key milestone:** By the end of this phase, the project had its four pillars
established: a C build system, a public API definition, communication standards,
3D assets, and a web app skeleton.

---

## Chapter 2 — Building the Ray Engine (January 22 – 28)

This was the most intense period of core engine development. The commit log
reads like a diary of rapid, sometimes messy, always forward-moving progress.

### Patryk's Simulation Sprint

Patryk drove the simulation engine with relentless focus:

- **Jan 22:** Created `AT_simulation.h/.c`, the simulation module, along with
  voxel grid structures and dynamic arrays
- **Jan 23:** Implemented `ray_triangle_intersect()` using the
  **Möller–Trumbore algorithm**, defined voxel functions, added
  `AT_voxel_bin_append()`, and wrote the first tests (`test_voxel.c`) — testing
  300 items in the voxel structure
- **Jan 25:** Started visual testing with **Raylib**, discovering that Raylib's
  internal CGLTF conflicted with their own — a linker error that led to the
  delightfully verbose commit: *"It seems that Raylib has its own implementation
  of CGLTF internally for their own models, so if we are testing using raylib we
  have to comment out the CGLTF implementation"*
- **Jan 26:** Landed `AT_RayHits` (PR #83) — the data structure for tracking
  where rays strike surfaces

### Eoghan's Geometry Work

In parallel, Eoghan was building the **AABB (Axis-Aligned Bounding Box)**
infrastructure that would eventually become the BVH acceleration structure:

- **Jan 26:** Implemented ray reflection (PR #78)
- **Jan 27:** Added model-to-AABB conversion (PR #94)

### Alex's Integration Push

Alex was the glue, connecting Patryk's simulation primitives with real 3D
models:

- **Jan 26:** Added `model_test`, removed external CGLTF in favour of cleaner
  integration
- **Jan 27:** A burst of six commits in one day — adding a free camera to the
  test viewer, fixing ray bounces, and iterating through frustrating bugs
  (`fix bounces!`, `fix again :/`, `remove MAX_BOUNCE_COUNT :(`)
- **Jan 28:** Added multiple primitive handling in `model_create`, new room
  models, and a screenshot feature for the test viewer

### Michael's TypeScript Migration

While the engine team worked in C, Michael undertook a **major migration** of
the web app from JavaScript to TypeScript:

- **Jan 28:** Three separate commits over the day — `Migrate from javascript to
  typescript`, `Migrate from javascript to typescript attempt 2`, and finally
  `Finished migration to typescript and simple upload form added`

The double attempt tells a familiar story — TypeScript migrations are never as
smooth as they look.

### The First Simulation Run

On **January 29**, Patryk landed the defining commit: **`AT_simulation_run()
simulating rays`** (PR #108). For the first time, the engine could emit rays
from a source point, trace them through a 3D environment, detect intersections
with triangles, and record the results. The acoustic tracer could *trace*.

---

## Chapter 3 — Acceleration & Physics (January 30 – February 10)

With basic ray tracing working, the team turned to making it fast and
physically accurate.

### The BVH Tree

Eoghan methodically built the **Bounding Volume Hierarchy**:

- **Jan 30:** Implemented AABB helper functions (PR #95)
- **Jan 31:** Removed double-declared functions — cleanup from rapid iteration
- **Feb 1:** Added midpoint calculations and linked triangles to AABBs (PR #115)
- **Feb 4:** Implemented **mini tree selection** (PR #120) — the algorithm for
  choosing how to split triangles into BVH nodes

This was careful, incremental work. Each PR built on the last, moving from
simple bounding boxes to a full spatial acceleration structure.

### Voxel DDA and Energy

Patryk tackled the **Digital Differential Analyzer** — the algorithm that steps
a ray through a 3D voxel grid, depositing energy in each cell it passes through:

- **Feb 4:** Landed Voxel DDA (PR #114) and immediately followed with a bugfix
  (`fixy fix voxl sim`, PR #125)
- **Feb 10:** Fixed voxel energy distribution and energy attenuation (PR #132)
  — ensuring that sound energy correctly diminished with distance and material
  absorption

Alex complemented this with material physics:

- **Feb 2:** Improved `AT_Material` with realistic absorption coefficients
- **Feb 2:** Added ray energy absorption — sound now lost energy when bouncing
  off surfaces

### Vector Refactor

Eoghan contributed a clean refactoring: **vectors as union of struct and array**
(PR #127) — allowing `vec.x` and `vec[0]` syntax interchangeably. A small
change that made the math code significantly more readable.

---

## Chapter 4 — The Web App Takes Shape (February 3 – 19)

### Appwrite Integration

On **February 3**, Michael landed a transformative PR: **Implement Web app with
Appwrite CRUD operations** (PR #111). The web app could now:

- Create and store simulation configurations
- Upload 3D model files
- List saved simulations in a dashboard

### Architecture Improvements

Michael then embarked on a series of architectural improvements:

- **Feb 6:** Started converting to **Tailwind CSS** (PR #124) — replacing
  hand-written CSS with utility classes
- **Feb 11:** Introduced **Zustand** for state management (PR #137) — giving
  the frontend a proper state layer
- **Feb 14:** Major file structure refactor following the
  [Bulletproof React](https://github.com/alan2207/bulletproof-react) pattern —
  adding `provider.tsx`, `router.tsx`, ErrorBoundary, and Suspense boundaries
- **Feb 18:** Replaced Zustand with **TanStack Query** for server state — a
  significant pivot that improved data fetching, caching, and error handling

The Zustand-to-TanStack migration happened over several commits on Feb 18:
implementing the simulation repository class, adding error fallbacks to all
routes, and refactoring the dashboard and scene components.

### The Networking Saga

Alex's journey to connect the C backend to the web frontend was an adventure in
itself:

- **Feb 9:** Added `AT_simulation_to_json` and integrated the **cJSON** library
- **Feb 18:** Added `AT_send_json_to_url` with **libcurl** — then promptly
  discovered the complexity it introduced
- **Feb 19:** Three commits in one day: `Delete libcurl :pray:`,
  `Semi-working`, `Remove all refs to libcurl` — a classic "add dependency,
  fight dependency, remove dependency" cycle
- **Feb 19:** Replaced the approach with a simpler **raytracer-api** backend

The `:pray:` emoji in the libcurl deletion commit speaks volumes.

---

## Chapter 5 — Source Placement & Interactivity (February 23 – March 1)

The simulation could now run, but users needed to *interact* with it — placing
sound sources, choosing materials, and seeing results.

### Voxel Interaction

Michael implemented the **click-to-place-source** workflow:

- **Feb 24:** Added `handlePick` — clicking on a voxel selects its centre as
  the source position
- **Feb 24:** Implemented the `selectedSource` store and refactored
  `scene.tsx` to use hooks (breaking up what he called a "god component")
- **Feb 25:** Created `source-marker.tsx` — a visual indicator showing where
  the sound source was placed

### Material & Direction

Alex added the configuration UI:

- **Feb 25:** Material selection from the front-end
- **Feb 25:** Source direction and position from the client
- **Feb 26:** Fixed circle drawing for the direction indicator

### Visual Polish

Alex and the team added rendering features:

- **Feb 28:** Replay feature for re-watching simulations
- **Mar 1:** Toggle model textures, wireframe display
- **Mar 1:** Colour-coded materials in the 3D view

### Scattering

On **March 1**, Patryk added **ray scattering** (PR #148) — modelling how sound
diffuses when hitting rough surfaces, rather than reflecting perfectly. This
made the simulation significantly more physically realistic.

---

## Chapter 6 — The Final Sprint (March 2 – 8)

The last week was an all-hands push to tie everything together.

### March 2–3: Integration Storm

- Michael fixed source marker and direction bugs, fixed merge issues, and
  implemented correct state loading for saved simulations
- Michael migrated `rayResponse` data from Zustand to TanStack Query, adding
  proper cache-based loading
- Eoghan landed **mini tree construction** (PR #138) — completing the BVH
  implementation
- Alex added the **playback feature** in the core engine

### March 4: The Big Day

**Fifteen commits landed on March 4** — the single busiest day in the project:

- Eoghan: Project saving, multiple node support, transformation matrices for
  model loading (PR #177)
- Alex: Fixed `num_voxels` Appwrite column issue, added material to model,
  fixed voxel heatmap colours
- Michael: UI overhaul (home page, config panel fixes, side panel transitions),
  proper loading from TanStack cache, toggle wireframe/texture fix, table rename
  action, dynamic source marker scaling, playback button, binary parsing

### March 5–7: Polish and Authentication

- Alex slowed the simulation speed and added a bathroom model (PR #180)
- Michael implemented the full **authentication flow**: login/register UX
  improvements, Google login integration, login/register handling in the user
  store, new auth pages, code review fixes, and a UI facelift with an updated
  toggle button and favicon

### March 7: The Last Commits

Michael's final commits polished the last details — updated imports, a cooler
toggle button, a fixed import, and a new **favicon**. The project had its face.

---

## By the Numbers

| Metric | Count |
|--------|-------|
| Total commits (including merges) | 307 |
| Non-merge commits | 212 |
| Merged pull requests | 85 |
| Contributors | 4 |
| Days of active development | 40 |
| First commit | January 14, 2026 |
| Latest commit | March 7, 2026 |

### Commits per Contributor

| Contributor | Non-Merge Commits | Primary Focus |
|-------------|-------------------|---------------|
| Patryk Mrozek | ~57 | Simulation engine, voxels, DDA |
| Alex Wright | ~58 | Core engine, networking, integration |
| Michael McCarthy | ~62 | Web frontend, UI/UX, auth |
| Eoghan Murphy | ~36 | BVH, models, documentation |

### Busiest Days

| Date | Commits | What Happened |
|------|---------|---------------|
| Jan 14 | 18 | Project inception, proposals, contributor setup |
| Jan 22 | 18 | Simulation module, web skeleton, file restructure |
| Jan 23 | 18 | Ray-triangle intersection, voxel functions, tests |
| Mar 4 | 15 | UI overhaul, model loading, project saving, heatmap |
| Jan 28 | 14 | TypeScript migration, rooms, source fixes |

---

## Technical Evolution Timeline

```
Jan 14    ┃ proposal.tex — the idea is born
          ┃
Jan 19    ┃ CMakeLists.txt — the C project compiles
          ┃
Jan 20-22 ┃ at.h, at_math.h, at_simulation.h — API defined
          ┃ web/ scaffolded with stubs and TODOs
          ┃
Jan 23    ┃ ray_triangle_intersect() — Möller–Trumbore implemented
          ┃ test_voxel.c — first unit tests
          ┃
Jan 25-26 ┃ Raylib visual tests — rays drawn in 3D for debugging
          ┃ Ray reflection implemented
          ┃
Jan 27    ┃ Ray bouncing works (after several "fix" commits)
          ┃
Jan 29    ┃ AT_simulation_run() — first full simulation execution
          ┃
Jan 30-   ┃ AABB helpers → BVH tree construction
Feb 4     ┃ Voxel DDA — energy deposited along ray paths
          ┃
Feb 2     ┃ Material absorption — physics gets real
          ┃
Feb 3     ┃ Appwrite CRUD — web app stores simulations
          ┃
Feb 9-19  ┃ JSON serialisation → libcurl saga → raytracer-api
          ┃
Feb 14    ┃ Bulletproof React refactor — frontend architecture
          ┃
Feb 18    ┃ Zustand → TanStack Query migration
          ┃
Feb 24-25 ┃ Click-to-place source, source markers
          ┃
Mar 1     ┃ Scattering, texture toggles, material colours
          ┃
Mar 3     ┃ BVH complete, playback feature, state persistence
          ┃
Mar 4     ┃ The Big Day — 15 commits, full integration
          ┃
Mar 7     ┃ Auth, Google login, final UI polish, favicon
```

---

## Recurring Themes

### 1. Iterate Fast, Fix Faster

The commit log is full of rapid-fire iteration. Alex's January 27 sequence —
`fix bounces!` → `fix again :/` → `remove MAX_BOUNCE_COUNT :(` — captures the
debugging rhythm. Patryk's `fixy fix voxl sim` and `voxel fixington` are
similarly honest about the messy reality of getting physics simulations to work.

### 2. Add It, Then Remove It

The libcurl saga is the clearest example, but the pattern repeats: external
CGLTF was added then removed in favour of a bundled version, Zustand was adopted
and then partially replaced by TanStack Query, MAX_BOUNCE_COUNT was introduced
and then removed. Good software is made by trying things and having the courage
to undo them.

### 3. Parallel Workstreams

The team rarely blocked each other. While Patryk built the simulation loop,
Eoghan worked on BVH acceleration, Alex integrated models and networking, and
Michael built the entire web frontend. The fork-based PR workflow they
established on day one made this parallel development possible.

### 4. Tests as Exploration

The C test files (`test_voxel.c`, `test_model.c`, `test_ray_reflect.c`,
`test_current_state.c`, `test_net.c`) weren't traditional unit tests — they
were **visual, exploratory tests** using Raylib to render rays, intersections,
and bounces in 3D. This approach let the team *see* their physics and catch bugs
that pure numerical tests might miss.

### 5. Documentation First, Then Code

The team wrote `workflow.md`, `c_conventions.md`, `communication_standards.md`,
and `projectsummary.md` before most of the code existed. These documents created
shared understanding and reduced the coordination cost of four people working on
deeply interconnected systems.

---

## Epilogue

AcousticTracer went from a seven-line LaTeX file to a full-stack application
with:

- A **C simulation engine** implementing ray tracing with Möller–Trumbore
  intersection, BVH acceleration, DDA voxel traversal, material absorption, and
  scattering
- A **React/TypeScript frontend** with Three.js 3D rendering, TanStack Query
  state management, Appwrite backend, Google authentication, and simulation
  playback
- A **library of 3D room models** created in Blender — from simple cubes to
  L-shaped rooms, polygonal spaces, cathedrals, and bathrooms
- **Comprehensive documentation** including coding conventions, communication
  standards, and a detailed project summary

The git log tells a story that every software team knows: bold plans, messy
middles, late-night fixes, triumphant breakthroughs, and the steady accumulation
of small changes into something that works. The emoticons in the commit messages
— `:pray:`, `:/`, `:(` — are the punctuation of real engineering, where things
break before they work and every fix is a small victory.

---

*This narrative was generated from analysis of the complete git history of the
[Acoustic-Resonance/AcousticTracer](https://github.com/Acoustic-Resonance/AcousticTracer)
repository (307 commits, 85 PRs, 4 contributors, January 14 – March 8, 2026).*
