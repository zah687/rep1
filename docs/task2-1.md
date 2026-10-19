# Task 2.1 — The Website That Builds

**Classroom timing:** 8 minutes to investigate, then an instructor-led solution reveal. You do not need to finish every step before the reveal.

[← Task 1.3](task1-3.md) · [Lab Home](../README.md) · [Next: Task 2.2 →](task2-2.md) · [📎 Cheatsheet](cheatsheet.md)

**Need help?** [Jump to the expandable hints](#-stuck-open-a-hint-click-to-expand). Open one at a time.

## Mission

You've investigated a command-line program. Now the case moves to a **website**: a tiny C HTTP server based on Umid's Docker lab.

The pipeline can compile the server and build its Docker image. But a green build leaves a crucial question unanswered: **can someone actually reach the website?**

For now, investigate the build process and make its evidence visible. You'll test the real HTTP response in Task 2.2.

- Open `website/server.c`. Find `PORT` and `MESSAGE`. You **do not** need to understand the socket code.
- Open `website/Dockerfile`. Identify how the executable is compiled, which port the server uses, and which command starts it.
- Open `.github/workflows/ci.yaml`. Find the two existing steps: `Compile web server` and `Build website image`.
- Add a step **after the compile step** that confirms the executable exists. Use `test -x /tmp/sdo-server`.
- Commit a change under `website/` (or to `.github/workflows/ci.yaml`), push, and inspect the workflow run.

**Don't fix the message in `server.c` yet.** You'll need that failure as evidence in Task 2.2.

## Evidence to collect

- Which step compiles the server directly on the GitHub Actions runner?
- Which step builds the Docker image, and what is its tag?
- Does a green build tell you whether `http://localhost:8080/` responds?
- What happens if the compiler succeeds but the executable is not where a later step expects it?
- Does `EXPOSE 8080` publish port 8080 on the runner?

## Reference desk

### A. A step is not a job

A workflow contains **jobs**, and each job contains ordered **steps**. Steps in the same job run on the same runner and can access files created by earlier steps.

```yaml
- name: Compile example
  run: gcc -o /tmp/example example.c

- name: Verify executable
  run: test -x /tmp/example
```

`test -x FILE` succeeds only if the file exists and is executable. Replace the example paths with those already used by the website workflow.

### B. Two builds, two environments

The workflow compiles the server once on the **runner**, then the Dockerfile compiles it again **inside the image build**.

```text
GitHub Actions runner                 Docker build
gcc ... -o /tmp/sdo-server            docker build -t sdo-web:1.0 ./website
                                      └─ Dockerfile compiles server.c
```

Passing either compilation check says nothing about whether the running server returns the right HTTP body.

### C. Read the Dockerfile

| Instruction | Meaning |
| --- | --- |
| `COPY server.c .` | Copies source into the image build context |
| `RUN gcc -o server server.c` | Compiles during the image build |
| `EXPOSE 8080` | Documents the container's listening port; **does not** publish it |
| `CMD ["./server"]` | Command the container starts by default |

The build context is `./website`, not the repository root.

### D. Find your workflow run

Go to your repository's **Actions** tab and select **CI/CD Pipeline Investigation**. The shared workflow runs on pushes to `main` and pull requests. You can inspect the `website` job in the same run as Expo.

## 🧩 Stuck? Open a hint (click to expand)

<details>
<summary>Hint 1 — Where does the new verification step go?</summary>

Put it directly after `Compile web server`, inside the existing `website` job. Don't create another job for this check.

</details>

<details>
<summary>Hint 2 — What exactly should be checked?</summary>

Look at the `-o` flag in the existing `gcc` command. It tells you the executable's path. `test -x` checks that file.

</details>

<details>
<summary>Hint 3 — My workflow did not run</summary>

Check that you committed and pushed to `main` (or opened a pull request). The shared workflow has no `paths:` filter.

</details>

## Completion criteria

- [ ] You identified the source, Dockerfile, and website workflow.
- [ ] The workflow explicitly checks that its compiled executable exists.
- [ ] Both compile and Docker image build steps pass.
- [ ] You can explain why a successful build **does not** prove the website responds.
- [ ] The intentionally incorrect website message is still present for the next task.

---

[← Task 1.3](task1-3.md) · [Lab Home](../README.md) · **[Next: Task 2.2 — A Green Build, a Broken Website →](task2-2.md)**
