# Team Workflow (Forks + Upstream + Draft PRs)

Upstream repository: `Acoustic-Resonance/AcousticTracer`  
Default branch: `main`  
Merge strategy: **Merge commit**  
Review requirement: **1 approval**  
PRs should start as: **Draft**

---

## 1) Roles and Definitions

- **Upstream**: `Acoustic-Resonance/AcousticTracer` (shared team repo)
- **Fork**: each member’s GitHub copy of the upstream repo
- **origin**: your fork remote (what you push to)
- **upstream**: the team repo remote (what you pull from)

Rule: **Nobody pushes to `upstream/main` directly.** Everything goes through PRs.

---

## 2) One-time Setup (each teammate)

### Fork on GitHub

Fork `Acoustic-Resonance/AcousticTracer` into your own account.

### Clone your fork

```bash
git clone https://github.com/<your-username>/AcousticTracer.git
cd AcousticTracer
```

### Add upstream remote

```bash
git remote add upstream https://github.com/Acoustic-Resonance/AcousticTracer.git
git remote -v
```

You should see:

- `origin` → your fork
- `upstream` → Acoustic-Resonance/AcousticTracer

---

## 3) Starting Work (always sync first)

Before creating a new branch, sync your `main`:

```bash
git switch main
git pull upstream main
git push origin main
```

---

## 4) Create a Branch (never work on `main`)

Branch naming:

- `feature/<short-description>`
- `fix/<short-description>`
- `docs/<short-description>`

Create:

```bash
git checkout -b feature/<short-description>
```

Commit early and often:

```bash
git add -A
git commit -m "Short, clear message"
```

Push your branch to your fork:

```bash
git push origin branch-name
```

---

## 5) Open a PR (Draft first)

Open a PR **from your fork branch → `Acoustic-Resonance/AcousticTracer:main`**.

### Draft PR rule

Open the PR as **Draft** until:

- you’ve done a self-review
- the PR description is complete

When ready:

- click **Ready for review**
- request **1 reviewer**
- address feedback by pushing more commits to the same branch

---

## 6) After Merge (everyone syncs)

```bash
git switch main
git pull upstream main
git push origin main
```

Then delete branches:

```bash
git branch -d feature/<short-description>
git push origin --delete feature/<short-description>
```

---

## 7) Repo Conventions (for this project)

### Commit Conventions

Commits should be written in the imperative, with the first letter capitalised.
The title of the commit should be less than 50 characters, any extra information should be in the commit body.

### Project Structure

<pre><code style="color: white; font-size: 12pt">.
├── core <em><sub>{Core C code for render calculations}</sub></em>
│   └── src <sub><em>{Core C simulation code}</sub></em>
│   └── core_dev <sub><em>{Debug code for the simulation}</sub></em>
│   └── external_libs <sub><em>{External libraries}</sub></em>
├── render <em><sub>{Code for rendering the scene}</sub></em>
│   └── web <sub><em>{Backend code for web app}</sub></em>
│   └── cli <sub><em>{Backend code for the terminal app}</sub></em>
├── app <em><sub>{Code for web/C-ui frontend application}</sub></em>
├── genetics <em><sub>{Code for optimizing the speaker placement}</sub></em>
├── assets <em><sub>{Assets folder for code}</sub></em>
│   └── stl <sub><em>{Object files defining points}</sub></em>
├── docs <em><sub>{Documentation for the project}</sub></em>
└── contributors <em><sub>{Files specifying every ones contributions}</sub></em>
</code></pre>
