# Lab Cheatsheet — Git, GitHub Actions & Local Checks

[← Lab Home](../README.md) · [Task 1](task1-1.md) · [Task 2](task1-2.md) · [Task 3](task1-3.md) · [Bonus](task3-bonus.md)

**Keep this page open while investigating.** It is a reference, not a solution key. You do **not** need Docker, Minikube, Jenkins, or even a C compiler installed locally to complete the main lab: GitHub Actions runs the checks on its own runner.

## 1. Your usual workflow

1. Read the current task and inspect the relevant file.
2. Make **one focused change**.
3. Optionally run a quick local check (see below).
4. Review your changes, commit, and push.
5. Open **GitHub → repository → Actions → latest workflow run**.
6. Open the failed job and step, read the logs, and investigate. Repeat.

> A red workflow is useful evidence. Don't change several unrelated files at once just to make it green.

## 2. Git commands — quick reference

Run these commands from the **repository root** (the directory containing `README.md`).

| Command                               | What it does                              |
| ------------------------------------- | ----------------------------------------- |
| `git status`                          | Show changed, staged, and untracked files |
| `git diff`                            | Review changes not yet staged             |
| `git diff --staged`                   | Review what will go into your next commit |
| `git add path/to/file`                | Stage a specific file                     |
| `git commit -m "Describe the change"` | Save a commit                             |
| `git push`                            | Send your commits to the remote branch    |
| `git branch --show-current`           | Show your current branch                  |
| `git log -1 --oneline`                | Show your latest commit                   |

**Example sequence** (replace the filename and commit message with your actual change):

```bash
git status
git diff
git add .github/workflows/ci.yaml
git diff --staged
git commit -m "Add a CI check"
git push
```

If `git push` says the branch has no upstream, Git usually prints the command needed to set it. Do not force-push to solve an ordinary error.

### Editing directly on GitHub?

You can edit a file in the browser and select **Commit changes**. That creates a commit without running local Git commands. Then open the **Actions** tab to inspect the result.

## 3. Before you push — 30-second checklist

- [ ] I know **which task** this change is for.
- [ ] I changed the intended file, not an unrelated one.
- [ ] I checked indentation in `.yml` files (use spaces, not tabs).
- [ ] I did not accidentally commit a compiled `sdo-expo` binary, `.DS_Store`, or temporary files.
- [ ] I checked `git diff` (or the GitHub editor's change preview).
- [ ] I committed and pushed to the intended branch.

**Branch reminder:** the lab's CI runs on pushes to `main` and on pull requests. If you push only to another branch, check whether you need to open a pull request. The optional Jenkins validator is configured for changes to the root `Jenkinsfile` pushed to `main`.

## 4. Optional local checks

These checks are **optional**. If a tool is not installed, use the GitHub Actions runner instead.

### C — if GCC is installed

```bash
gcc expo/expo.c -lm -o sdo-expo
./sdo-expo 2 10
```

Look at the printed result. Compiling successfully does **not** prove the calculation is correct.

### Docker — if Docker is installed

```bash
docker images
docker ps -a
```

Use these commands to inspect local images and containers. The GitHub Actions workflow already builds the lab image using Docker Buildx; you do not need to reproduce that setup locally.

### Kubernetes — if you already have a cluster

```bash
kubectl get jobs,pods
kubectl describe job JOB_NAME
kubectl logs job/JOB_NAME
```

Replace `JOB_NAME` with the name from `expo/Job.yaml`. Do **not** run these commands against an unrelated cluster. For this lab, the intended Kubernetes environment is the temporary Minikube cluster created in GitHub Actions.

## 5. Finding the useful GitHub Actions log

1. Open your repository on GitHub.
2. Select **Actions**.
3. Open the workflow run for your latest commit or pull request.
4. Select the job that failed or was skipped.
5. Expand the relevant step and read the first meaningful error message.

| What you see                       | First question to ask                                               |
| ---------------------------------- | ------------------------------------------------------------------- |
| **Green check**                    | Did the workflow actually verify the behavior required by the task? |
| **Red X**                          | Which step failed, and what does its error say?                     |
| **Skipped job**                    | Does the job depend on another job that failed?                     |
| **No new run**                     | Did you push? Which branch? Does the workflow trigger on it?        |
| **Build succeeds, runtime fails**  | Does the image launch the correct executable?                       |
| **Kubernetes Job never completes** | What do the Pod status and events say?                              |

## 6. GitHub Actions YAML — tiny reference

```yaml
jobs:
  example:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Show current directory
        run: pwd
```

- `jobs:` defines jobs; each job has its own runner by default.
- `steps:` run in order **inside the same job**.
- `uses:` runs an existing GitHub Action.
- `run:` executes shell commands.
- `needs:` declares a dependency **between jobs**.
- `if:` controls when a job or step runs.
- `run: |` allows multiple shell lines.

**Docker note:** `docker/build-push-action@v6` with `load: true` makes the built image available to the runner's Docker Engine. Minikube still needs the image imported into its own environment.

## 7. Common mistakes

- **`git push` says “Everything up-to-date”** — Check whether you committed your changes (`git status`).
- **GitHub Actions did not run** — Confirm the branch and workflow triggers; check the Actions tab.
- **YAML parsing error** — Check indentation, colons, and whether a step begins with `-`.
- **`./sdo-expo` not found locally** — Compile it first, or use GitHub Actions.
- **Docker command not found** — Local Docker is optional; inspect the CI run instead.
- **No Kubernetes logs** — The Pod may never have started; inspect its status and events.
- **The output is unexpected** — Print the actual output before changing the expected value.

## 8. Need a task-specific hint?

Return to [Task 1.1](task1-1.md), [Task 1.2](task1-2.md), [Task 1.3](task1-3.md), [Task 2.1](task2-1.md), [Task 2.2](task2-2.md), [Task 2.3](task2-3.md) or the [Jenkins Bonus](task3-bonus.md) and open its expandable hints **one at a time**.

---

[← Back to Lab Home](../README.md)
