# Task 1.3 — Kubernetes: Follow the Evidence

**Classroom timing:** 10 minutes to investigate, then an instructor-led solution reveal. You do not need to finish every step before the reveal.

[← Task 1.2](task1-2.md) · [Lab Home](../README.md) · [📎 Cheatsheet](cheatsheet.md) · [Next: Task 2.1 →](task2-1.md)

**Goal:** Extend the existing `docker-build` job to run your image in Kubernetes, then diagnose why the Kubernetes Job does not complete.

> You are **not** expected to invent the pipeline structure from scratch. Follow the sequence below, fill in the values from your repository, and use the logs to investigate the failure.

## Part A — Connect the steps (guided, ~8 minutes)

You already have an image named `sdo-expo:1.0` built with Docker Buildx. Because the build action uses `load: true`, that image is available to Docker on the GitHub Actions runner. Minikube runs a Kubernetes cluster with its own image environment, so the image must also be loaded into Minikube.

Add the following steps **after your Docker container test, inside the same `docker-build` job**. Keep them in this order:

| Order | Step                | Why it comes here                                                       |
| ----- | ------------------- | ----------------------------------------------------------------------- |
| 1     | Start Minikube      | `kubectl` needs a running cluster                                       |
| 2     | Load the image      | The cluster needs access to the image before starting a Pod             |
| 3     | Apply the manifest  | Creates the Kubernetes Job                                              |
| 4     | Wait for completion | `apply` creates the Job; it does **not** wait for the program to finish |
| 5     | Check logs          | A completed Job must also produce the correct result                    |
| 6     | Collect diagnostics | When something fails, inspect the cluster instead of guessing           |

Use this **scaffold**, replacing the placeholders with values you find in `.github/workflows/ci.yaml` and `expo/Job.yaml`:

```yaml
- name: Start Minikube
  uses: medyagh/setup-minikube@latest

- name: Load image into Minikube
  run: minikube image load IMAGE:TAG

- name: Create Kubernetes Job
  run: kubectl apply -f MANIFEST_FILE

- name: Wait for Job completion
  run: |
    kubectl wait \
      --for=condition=complete \
      --timeout=120s \
      job/JOB_NAME
```

**Where do the placeholders come from?**

- `IMAGE:TAG`: the image produced by your Docker Buildx step.
- `MANIFEST_FILE`: the Kubernetes YAML file in the repository root.
- `JOB_NAME`: the value of `metadata.name` in that manifest.

The `\` at the end of a Bash line means the command continues on the next line. The `|` after `run:` allows a multi-line shell script in GitHub Actions YAML.

## Part B — Turn logs into a test (~4 minutes)

Create another GitHub Actions step after `kubectl wait`. It must read the Job logs and fail if the program does not print **exactly**:

```text
2 ^ 10 = 1024
```

You already used this pattern in Tasks 1 and 2:

```bash
output=$(SOME_COMMAND)
echo "Observed: $output"
test "$output" = "EXPECTED_OUTPUT"
```

Replace `SOME_COMMAND` with the appropriate `kubectl logs` command from the reference below. Kubernetes resources are addressed as `job/JOB_NAME` (with a **forward slash**).

**Why both `wait` and `logs`?** A completed Job tells you the process finished successfully; checking its logs verifies the actual answer.

## Part C — Make failures visible (~3 minutes)

A failed `kubectl wait` normally causes GitHub Actions to skip later ordinary steps. That can hide the information you need to debug.

Add a **separate diagnostic step at the end** of the `docker-build` job:

```yaml
- name: Kubernetes diagnostics
  if: failure()
  run: |
    kubectl get jobs,pods || true
    kubectl describe job JOB_NAME || true
    kubectl describe pods -l job-name=JOB_NAME || true
    kubectl logs job/JOB_NAME --all-containers=true || true
```

Two new pieces of syntax:

- **`if: failure()`** is a **GitHub Actions condition**. It runs this step when an earlier step in the job has failed.
- **`|| true`** is **shell syntax**. If a diagnostic command fails (for example, because the Pod does not exist), the script continues to the next command instead of stopping.

**Do not add `|| true` to your functional test or `kubectl wait`.** Those failures must remain visible so the workflow turns red when something is wrong.

## Part D — Investigate the failure (~5 minutes)

Commit and push the workflow. Open **Actions → CI/CD Pipeline Investigation → docker-build** and inspect the failed step and the diagnostics output.

If the Job does not complete:

1. Find the Pod status in `kubectl get jobs,pods`.
2. Read the Pod **Events** in `kubectl describe pods`.
3. Compare the image references in **three places**: the Docker Buildx `tags`, the `minikube image load` command, and the `image:` field in `expo/Job.yaml`.
4. Change the underlying configuration only after identifying the mismatch.
5. Push again. Confirm that the Job completes **and** the output check passes.

**Useful distinction:** If a Pod cannot start, there may be no application logs. Read its status and Events first; do not assume `kubectl logs` will explain everything.

## Quick command reference

| Command                                                         | What it tells you                                   |
| --------------------------------------------------------------- | --------------------------------------------------- |
| `kubectl apply -f FILE`                                         | Create/update resources from a manifest             |
| `kubectl get jobs,pods`                                         | Which Jobs and Pods exist, and their current status |
| `kubectl describe pods`                                         | Why a Pod may be waiting or failing (see Events)    |
| `kubectl wait --for=condition=complete job/NAME --timeout=120s` | Whether the Job completes within two minutes        |
| `kubectl logs job/NAME`                                         | What the application printed                        |
| `minikube image load IMAGE:TAG`                                 | Import an existing local image into Minikube        |

## 🧩 Stuck? Open a hint

<details>
<summary>Hint 1 — Where do I put the Kubernetes steps?</summary>

Inside the existing `docker-build` job, after the Docker runtime test. Do not create a separate job: the locally built image would not automatically be available on another runner.

</details>

<details>
<summary>Hint 2 — `kubectl wait` times out</summary>

Check `kubectl get jobs,pods`, then look for image-related messages in the Pod Events. Compare the exact image **name and tag** in the build, image-load step, and manifest.

</details>

<details>
<summary>Hint 3 — The logs step fails even though the Job exists</summary>

Check whether the Job actually completed and whether its Pod started. The resource syntax is `job/NAME`, not `job\\NAME`.

</details>

<details>
<summary>Hint 4 — My diagnostic step is skipped</summary>

The `if: failure()` condition belongs on the diagnostic **step**, at the same indentation level as `name:` and `run:`. Keep it separate from the step that waits for completion.

</details>

<details>
<summary>Hint 5 — What exactly should I change to fix the image issue?</summary>

Inspect `image:` in `expo/Job.yaml` and compare it with `tags:` in the Docker Buildx step. With `imagePullPolicy: Never`, Kubernetes will only use an image that has already been loaded into Minikube under the requested name and tag.

</details>

## Completion criteria

- [ ] Kubernetes steps run in the correct order inside `docker-build`.
- [ ] The image is loaded into Minikube before the Job starts.
- [ ] The Job is applied and its completion is checked.
- [ ] The Job's **actual output** is tested against `2 ^ 10 = 1024`.
- [ ] A diagnostic step runs when Kubernetes verification fails.
- [ ] The image configuration problem was identified using evidence and corrected.
- [ ] The final GitHub Actions run passes.

---

[← Task 1.2](task1-2.md) · [Lab Home](../README.md) · [📎 Cheatsheet](cheatsheet.md) · [Next: Task 2.1 →](task2-1.md)
