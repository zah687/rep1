# Task 3 — Bonus: Jenkins Pipeline Challenge

**Optional challenge:** approximately 15–20 minutes  
[← Task 3](task1-3.md) · [Lab Home](../README.md) · [📎 Cheatsheet](cheatsheet.md)

**Need help?** [Jump to the expandable hints](#-stuck-open-a-hint-click-to-expand). Open one at a time.

## Your mission

You already built a working CI pipeline in GitHub Actions. Now recreate its core checks using **Jenkins Declarative Pipeline**.

**You only need to edit the repository-root `Jenkinsfile`.** Do not edit `.github/workflows/jenkins-check.yml` or try to install Jenkins yourself.

The starter `Jenkinsfile` already contains a **Compile** stage. Add:

1. **Functional Test** — Run the compiled C program with `2` and `10`; fail the stage unless the output is exactly `2 ^ 10 = 1024`.
2. **Docker Build** — Build the existing Dockerfile into an image tagged `sdo-expo:1.0`.

**Extra challenge:** Add a **Docker Test** stage that runs that image with `2` and `10` and checks the output. This is optional; the two stages above are the core bonus task.

## How your work is checked

1. Save your changes to `Jenkinsfile`.
2. Commit and push them to **`main`**.
3. Open your repository's **Actions** tab.
4. Open the workflow named **Jenkins Bonus Validation**.
5. Expand its steps to inspect the Jenkins execution and console output.

This validator is configured to trigger **only when the root `Jenkinsfile` changes on `main`**. Editing `expo/expo.c` or `docs/task3-bonus.md` alone will not start a new Jenkins validation run.

> **Important:** A GitHub Actions job becoming green is not enough if you accidentally removed the required test. Check that Jenkins really executed **Compile**, **Functional Test**, and **Docker Build**.

## A translation guide: GitHub Actions → Jenkins

| GitHub Actions           | Jenkins Declarative Pipeline | Meaning                       |
| ------------------------ | ---------------------------- | ----------------------------- |
| `jobs:`                  | `pipeline { ... }`           | Define the pipeline           |
| `runs-on: ubuntu-latest` | `agent any`                  | Select an available executor  |
| `steps:`                 | `steps { ... }`              | Commands to execute           |
| `- name: Compile`        | `stage('Compile') { ... }`   | A named section shown in logs |
| `run: gcc ...`           | `sh 'gcc ...'`               | Run a shell command           |
| A failing `run:` command | A failing `sh` command       | Mark the stage as failed      |

Jenkins stages execute in the order they appear in `stages { ... }` in this simple pipeline. You do **not** need to add a GitHub Actions-style `needs:` declaration.

## Reference desk

### A. Understand the nesting

A Declarative Pipeline uses this structure:

```groovy
pipeline {
    agent any

    stages {
        stage('Example') {
            steps {
                sh 'echo Hello from Jenkins'
            }
        }
    }
}
```

- `pipeline` is the outermost block.
- `agent any` asks Jenkins for an available executor.
- `stages` contains one or more named `stage` blocks.
- Each `stage` contains a `steps` block.
- `sh` executes a shell command on the Jenkins agent.

**Common mistake:** A new `stage(...)` must be placed **inside `stages`**, but **outside** the previous stage's closing brace.

### B. Add another stage without breaking the braces

This unrelated example has two valid stages:

```groovy
pipeline {
    agent any

    stages {
        stage('Greeting') {
            steps {
                sh 'echo Hello'
            }
        }

        stage('Check Tool') {
            steps {
                sh 'gcc --version'
            }
        }
    }
}
```

Count opening and closing `{ }` braces if Jenkins reports a syntax error.

### C. Use single-line or multi-line shell commands

A single command can be written as:

```groovy
sh 'echo Hello'
```

For multiple commands, use **triple single quotes**:

```groovy
sh '''
    value=$(printf 'hello')
    echo "Observed: $value"
    test "$value" = "hello"
'''
```

Triple single quotes let the **shell** interpret `$value`. Avoid Groovy double-quoted strings for this Bash pattern, because `$` can be interpreted by Groovy before the shell runs.

### D. Reuse what you learned in the main lab

You already know how to:

- Compile the C application using GCC.
- Execute it with command-line arguments.
- Capture output with `$(...)` and compare strings with `test`.
- Build a Docker image with `docker build -t IMAGE:TAG ./expo`.
- Optionally run a container with `docker run --rm IMAGE:TAG ARG1 ARG2`.

The new challenge is **where those commands belong in Jenkins syntax**, not inventing new shell commands.

### E. Keep the stages in a useful order

The compiled executable is created by the existing Compile stage. Your Functional Test stage should run **after** Compile. The Docker Build stage should run after the functional test so that incorrect application behavior prevents the build from proceeding.

In this lab's Jenkins validator, the repository files are prepared in the Jenkins workspace. You do **not** need to add `git clone`, `checkout scm`, credentials, a registry login, a Kubernetes stage, or a deployment step.

## Evidence to collect

When you open the Jenkins validation logs, check:

- Does Jenkins recognize your Declarative Pipeline syntax?
- Does it enter each required stage?
- Does GCC compile successfully?
- Does the functional test show the actual application output?
- If a stage fails, which `sh` command returned a nonzero status?
- Does Docker use the expected image tag?

## Common failures and what to inspect

| Symptom                                                | Likely area to investigate                                                           |
| ------------------------------------------------------ | ------------------------------------------------------------------------------------ |
| `WorkflowScript: ... expecting '}'`                    | Missing or misplaced Groovy brace                                                    |
| `Unknown stage section` or `Expected a stage`          | Incorrect nesting of `stage` and `steps`                                             |
| `No such DSL method 'sh'`                              | `sh` placed outside `steps`, or malformed syntax                                     |
| `./sdo-expo: not found`                                | Compile stage failed, executable name differs, or wrong working directory            |
| Test stage fails with an exit code                     | Compare actual output with the expected string                                       |
| `docker: command not found` or daemon connection error | Jenkins runner setup problem; ask the TA rather than editing the application's logic |
| Docker build fails                                     | Inspect the Dockerfile and the Docker build log                                      |
| Jenkins validation never appears in Actions            | Check whether the **root `Jenkinsfile`** changed in a push to **`main`**             |

## 🧩 Stuck? Open a hint (click to expand)

<details>
<summary>Hint 1 — Where do I put the new stages?</summary>

Place both new `stage(...)` blocks after the existing Compile stage, but still inside the `stages { ... }` block.

</details>

<details>
<summary>Hint 2 — How do I make the functional test fail on incorrect output?</summary>

Use the Bash pattern from Task 1: capture the executable's output, print it for debugging, and compare it with the expected string using `test`. Put those commands inside a Jenkins `sh ''' ... '''` block.

</details>

<details>
<summary>Hint 3 — How should the Docker stage look?</summary>

Use a new `stage('Docker Build')` with `steps { ... }`. Inside it, use `sh` to execute the same Docker build command you used in GitHub Actions.

</details>

<details>
<summary>Hint 4 — Jenkins fails before reaching my stage</summary>

Check the braces and quotation marks first. A Jenkinsfile is Groovy-based pipeline syntax, **not YAML**. Do not use `run:`, `uses:`, or `- name:` inside a Jenkinsfile.

</details>

<details>
<summary>Hint 5 — My Jenkinsfile is valid, but the validator itself fails to start</summary>

If Jenkins cannot download plugins, launch its server, or connect to the host Docker daemon **before your pipeline runs**, that is infrastructure rather than a student Jenkinsfile mistake. Show the TA the failed GitHub Actions step and its logs.

</details>

## Completion criteria

- [ ] I edited only the repository-root `Jenkinsfile` for this bonus.
- [ ] The existing Compile stage still works.
- [ ] A Functional Test stage checks `2 ^ 10 = 1024`.
- [ ] A Docker Build stage builds `sdo-expo:1.0`.
- [ ] The stages are in the correct order.
- [ ] Jenkins Bonus Validation executes the required stages successfully.
- [ ] **Optional:** A Docker Test stage verifies the container output.

---

[← Task 3 — Kubernetes Investigation](task1-3.md) · [Back to Lab Home](../README.md)
