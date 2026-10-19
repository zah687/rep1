# Task 2.3 — Reliability: Follow the Failure Evidence

**Classroom timing:** 10 minutes to investigate, then an instructor-led solution reveal. You do not need to finish every step before the reveal.

[← Task 2.2](task2-2.md) · [Lab Home](../README.md) · [Next: Jenkins Bonus →](task3-bonus.md) · [📎 Cheatsheet](cheatsheet.md)

**Need help?** [Jump to the expandable hints](#-stuck-open-a-hint-click-to-expand). Open one at a time.

## Mission

Your HTTP assertion works, but CI has a new weakness: **a correct website can fail the test if it hasn't started listening yet**.

A fixed `sleep 10` might be unnecessarily slow or still too short. Your goal is to make the pipeline **wait for readiness, fail when readiness never arrives, and collect useful evidence on failure**.

### Part A — Replace guessing with a readiness check (~8 min)

Add a readiness step **between** the background `docker run` step and the HTTP content assertion.

It must:

1. Attempt an HTTP GET to `http://localhost:8080/`.
2. Retry after a short pause if the server is not reachable.
3. Stop after a **bounded** number of attempts (e.g. 15 tries, 2 seconds apart).
4. **Fail the workflow** if none of the attempts succeeds.

Keep the Task 2.2 content assertion as a **separate step**. Readiness proves the endpoint is reachable; the assertion proves its answer is correct.

### Part B — Collect diagnostics (~5 min)

Add a **final step** in the same `website` job. It should run when a previous step fails, and display:

- All containers, including stopped ones.
- The website container's logs, if available.

Use `if: failure()` so the diagnostic step runs after a failure rather than being skipped. Use `|| true` **only** for best-effort diagnostic commands, never to hide test failures.

### Part C — Prove the pipeline can fail (~5 min)

Temporarily change the port mapping in your `docker run` step so that runner port `8080` no longer reaches the server. Push and inspect the run.

Expected evidence:

- The readiness step retries and eventually **fails**.
- The HTTP content assertion is **skipped** after that failure.
- The diagnostic step still runs and prints container information.

Restore the correct port mapping and push again. Your final run should be green.

## Evidence to collect

- How many attempts does the readiness step make before failing?
- Can the retry loop accidentally finish successfully when all requests fail?
- Which step proves reachability? Which step proves correct behavior?
- What do the diagnostic logs show when the server is unreachable?
- After restoring the port, does the pipeline recover?

## Reference desk

### A. Bounded retries

```bash
ready=false

for attempt in $(seq 1 15); do
  echo "Readiness attempt $attempt"
  if curl --fail --silent --show-error http://localhost:8080/ >/dev/null; then
    ready=true
    break
  fi
  sleep 2
done

test "$ready" = "true"
```

**Investigation clue:** The last line matters. Without it, a loop that never reaches the website can still exit with status `0` and turn the step green.

### B. GitHub Actions failure conditions

```yaml
- name: Website diagnostics
  if: failure()
  run: |
    docker ps -a || true
    docker logs sdo-web || true
```

`if: failure()` is a **GitHub Actions step condition**. It allows this step to run after an earlier failure in the job.

`|| true` is **Bash syntax**. It lets the diagnostic script continue if, for example, the container was never created.

### C. Why two separate HTTP checks?

| Check | Question | Failure example |
| --- | --- | --- |
| Readiness loop | Can the runner reach the server? | Wrong port, process exited, startup delayed |
| Content assertion | Does the server return the correct greeting? | Wrong `MESSAGE` in `server.c` |

Do **not** use `|| true` on `test "$ready" = "true"` or on the greeting assertion. That would make real failures look successful.

### D. A common YAML trap

All steps must be indented at the same level under `jobs.website.steps`. A step-level `if:` is aligned with `name:` and `run:`, **not** nested inside the shell script.

## 🧩 Stuck? Open a hint (click to expand)

<details>
<summary>Hint 1 — My loop is green even though the website never responded</summary>

A `for` loop does not automatically fail just because its `if` condition was false every time. Track whether any request succeeded, then check that result after the loop.

</details>

<details>
<summary>Hint 2 — My diagnostics are skipped after a failure</summary>

Put the diagnostics in their own step and add `if: failure()` at the **step level**. A normal later step is skipped after a failed required step.

</details>

<details>
<summary>Hint 3 — The pipeline is always green, even with the wrong port</summary>

Look for `|| true` on the readiness test or content assertion. Remove it from required checks. It belongs only on best-effort diagnostics.

</details>

## Completion criteria

- [ ] The readiness loop retries a bounded number of times.
- [ ] The readiness step fails when the server never responds.
- [ ] The HTTP content assertion remains separate and verifies the expected greeting.
- [ ] A failure triggers container diagnostics.
- [ ] You tested a deliberate port failure, inspected the logs, and restored the port.
- [ ] The final GitHub Actions run is green for the right reason.

---

[← Task 2.2](task2-2.md) · [Lab Home](../README.md) · **[Next: Task 3 — Optional Jenkins Bonus →](task3-bonus.md)**
