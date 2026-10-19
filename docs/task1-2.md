# Task 1.2 — The Unprotected Docker Build

**Classroom timing:** 9 minutes to investigate, then an instructor-led solution reveal. You do not need to finish every step before the reveal.

[← Task 1.1](task1-1.md) · [Lab Home](../README.md) · [Next: Task 1.3 →](task1-3.md) · [📎 Cheatsheet](cheatsheet.md)

**Need help?** [Jump to the expandable hints](#-stuck-open-a-hint-click-to-expand). Open one at a time.

## Mission

The C program now passes its functional test. The repository also contains a `docker-build` job. But two things remain unproven:

1. **Dependency:** Will Docker build even when the functional test fails?
2. **Runtime:** Does an image that builds successfully actually start and run the program?

### Part A — Protect the build

Inspect the two jobs in `.github/workflows/ci.yaml`. Make `docker-build` depend on a successful `test` job.

**Prove your change:** Temporarily make the functional test fail, push, and inspect the Actions run. The Docker job should be **skipped**, not executed. Restore the correct test afterward.

### Part B — Test the container

Add a step to the existing Docker job that starts the image with base `2` and exponent `10`. Check the observed output against `2 ^ 10 = 1024`.

The image may build while the container fails to start. If that happens, investigate the Dockerfile rather than repeatedly rebuilding it unchanged.

## Evidence to collect

- When the functional test fails, is `docker-build` skipped?
- Does the Docker **build** step pass?
- Does the Docker **run** step pass?
- If the container cannot start, which path does Docker attempt to execute?
- Where is the executable actually placed in the final image?

## Reference desk

### A. Job dependencies

Jobs run independently unless a dependency is declared. Here `publish` runs only after `verify` succeeds:

```yaml
jobs:
  publish:
    needs: verify
    runs-on: ubuntu-latest
```

Use the actual job IDs in your workflow; a display name is not necessarily a job ID.

### B. Docker command shapes

```text
docker build -t IMAGE_NAME:TAG BUILD_CONTEXT
docker run --rm IMAGE_NAME:TAG ARG1 ARG2
```

For Expo, the Dockerfile is in `expo/`, so the Docker build context is `./expo`.  The image name and tag must match between `docker build` and `docker run`.

### C. Read a multi-stage Dockerfile

In a multi-stage build, `COPY --from=build SOURCE DESTINATION` copies a file into the **final** image. `ENTRYPOINT` specifies what the container tries to execute when it starts.

Example using unrelated paths:

```dockerfile
COPY --from=build /build/tool /usr/local/bin/tool
ENTRYPOINT ["/usr/local/bin/tool"]
```

The path in `ENTRYPOINT` must exist **inside the final image**.

### D. Verify the result

A container returning exit code `0` is useful, but for this lab you must also verify the printed answer. You can reuse the Bash output-capture and comparison pattern from [Task 1](task1-1.md).

## 🧩 Stuck? Open a hint (click to expand)

<details>
<summary>Hint 1 — The Docker job still runs when the test is red</summary>

Look at the job-level `needs:` property, not a step-level property. It belongs directly under the `docker-build` job ID.

</details>

<details>
<summary>Hint 2 — Docker build is green, but Docker run fails</summary>

Compare the final-stage `COPY` destination with `ENTRYPOINT`. A Docker image can contain a valid executable and still point at the wrong location.

</details>

<details>
<summary>Hint 3 — Which path should the container start?</summary>

Follow the file through the **final** stage of the Dockerfile. The location used by `ENTRYPOINT` must exactly match the copied binary's destination.

</details>

## Completion criteria

- [ ] `docker-build` waits for `test`.
- [ ] A deliberately failing functional test causes Docker to be skipped.
- [ ] The Docker image builds.
- [ ] The container starts and executes the application.
- [ ] The container's actual output is checked against `2 ^ 10 = 1024`.
- [ ] The correct test is restored and the workflow passes.

---

[← Task 1.1](task1-1.md) · [Lab Home](../README.md) · [Next: Task 3 — Kubernetes Investigation →](task1-3.md)
