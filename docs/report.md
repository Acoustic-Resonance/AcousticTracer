# Acoustic Tracer

## Table of Contents

- Introduction
- Previous Works
- Overview of ours (basically the presentation)
- architecture (front & back)

### C

- Implementation

- Rays
  - Voxels
- Design choices (c library)
- Optimisation

- API comms (both, potent split into two sections)
  - bins

### Frontend

## Introduction and Motivation

The AcousticTracer project pairs a C simulation engine with a browser-based frontend, which configures, stores, and visualises the simulation. Tools like ODEON exist for this domain, but they are desktop-only, expensive, and closed-source. A browser-based alternative would be freely accessible, require no installation, and immediate real-time use. This section describes the architecture of that browser frontend: what its major components are, how they interact at runtime, and what was learned building this project while starting with little to no prior experience in Three.js, WebGL, or production React architecture.

### Core Goals

The frontend has three responsibilities, each with distinct technical demands:

1. **Configure and submit** — The user uploads a `.glb` 3D room model, sets simulation parameters (voxel size, ray count, FPS, surface material), and interactively places a sound source by clicking inside the 3D scene. The configuration is sent to the C backend as JSON.

2. **Decode and store** — The C backend returns simulation results as a custom binary format (`.atrb`). The frontend must decode this into typed arrays, store it in cloud file storage for persistent storage, and cache the parsed result so revisiting a completed simulation is instantaneous.

3. **Render and replay** — The decoded frames must be visualised as a 3D voxel heatmap overlaid on the room model, animated at the simulation's original FPS. A typical simulation produces tens of thousands to hundreds of thousands of voxels, each requiring per-frame position and color updates. This must run at interactive frame rates in a browser, with a fully functioning playback system.

- Design choices (front)
  - state storing
  - storing (not state)
  - rendering
  - replaying
  - auth
  - routing

- What we would do different (ODEON)
- Lessons learned
- Conclusions [^1]
- references

## References