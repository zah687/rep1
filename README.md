# CI/CD Lab — The Green Build That Lies

Welcome, investigators! Your repository contains a small C application and a CI pipeline. The initial build may look healthy, but **a successful build is not proof that the software works**.

Your mission is to gather evidence, uncover problems at different layers, and turn the pipeline into a reliable set of checks.

**Class duration:** 75 minutes total, including short student work periods and instructor-led solution reveals after **every** task. When time is called, follow the solution on the slides and move to the next case. Jenkins is an optional bonus.  
**Tools:** GitHub, GitHub Actions, C, Docker, Minikube, Kubernetes; Jenkins for the bonus.  
**No local installation required:** work in the repository and inspect GitHub Actions logs.

**[📎 Open the Lab Cheatsheet — Git commands, pre-push checklist, optional local checks & Actions debugging](docs/cheatsheet.md)**

## Mission map — 75-minute classroom schedule

Each **Try** period is time to investigate before the instructor shows the solution on the slides. You are **not expected to finish every step alone** within that time. Follow the reveal, ask questions, and move on together.

| Investigation | Try it | Solution reveal | Total |
| --- | ---: | ---: | ---: |
| Introduction & debugging toolkit | — | — | 5 min |
| **Task 1 — Expo** | | | |
| [1.1 — Functional Test](docs/task1-1.md) | 8 min | 2 min | 10 min |
| [1.2 — Docker](docs/task1-2.md) | 9 min | 2 min | 11 min |
| [1.3 — Kubernetes](docs/task1-3.md) | 10 min | 3 min | 13 min |
| **Task 2 — Website** | | | |
| [2.1 — Website Build](docs/task2-1.md) | 8 min | 2 min | 10 min |
| [2.2 — HTTP Response Check](docs/task2-2.md) | 9 min | 2 min | 11 min |
| [2.3 — Reliability & Diagnostics](docs/task2-3.md) | 10 min | 3 min | 13 min |
| Wrap-up | — | — | 2 min |
| **Total** | **54 min of introduction, practice & wrap-up** | **16 min** | **75 min** |

**Task 3 — [Jenkins Pipeline Challenge](docs/task3-bonus.md): optional bonus, outside the 75-minute classroom schedule.**

**Instructor pacing:** Keep each task slide visible during its Try period; show the corresponding solution slide immediately afterward. Do not wait for everyone to finish before moving on. Kubernetes/Minikube startup can take time, so use the reveal to discuss logs and commands even if a runner is still executing.

## Case files

| File | What it controls |
| --- | --- |
| `expo/expo.c` | The Expo program's behavior |
| `.github/workflows/ci.yaml` | **One workflow** with Expo and Website jobs |
| `expo/Dockerfile` | How the Expo image is assembled and launched |
| `expo/Job.yaml` | The Expo Kubernetes Job configuration |
| `website/server.c` | The website's HTTP server and response |
| `website/Dockerfile` | The website container image |
| `Jenkinsfile` | The optional Jenkins pipeline |
| `.github/workflows/jenkins-check.yaml` | Jenkins validation workflow |

## Rules of investigation

1. **Observe first.** Read the relevant file and the CI log before changing code.
2. **Form a hypothesis.** What should happen? What actually happens?
3. **Change one cause at a time.** Avoid editing several unrelated files to hide a failure.
4. **Verify the result.** A green check matters only when the intended behavior was tested.
5. **Use the references and expandable hints.** They explain the tools without handing you a complete solution.

### Working with GitHub Actions

Open your repository's **Actions** tab, select **CI/CD Pipeline Investigation**, then inspect the **test**, **docker-build**, or **website** job, open the latest run, then expand the relevant job and step. After editing files, commit and push to `main` to trigger a new run. If a step fails, read its **first useful error message** before retrying.

A failed run is evidence, not the end of the investigation.

**[Begin the investigation → Task 1.1](docs/task1-1.md)**

## Need a hint?

The Expo task pages include progressive hints; check the website task instructions and the cheatsheet for additional guidance. Start with the mission, try the task, and open a hint only when you get stuck.

## Website investigation

After finishing Expo, continue with **[Task 2.1 — Website Build](docs/task2-1.md)**, then [2.2](docs/task2-2.md) and [2.3](docs/task2-3.md). The website job runs in the **same GitHub Actions workflow** as Expo, but is an independent job.

**[Continue the investigation → Task 2.1](docs/task2-1.md)**

The [Jenkins challenge](docs/task3-bonus.md) is an optional bonus after the GitHub Actions tasks.
