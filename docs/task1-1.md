# Task 1.1 — The Green Build That Lies

**Classroom timing:** 8 minutes to investigate, then an instructor-led solution reveal. You do not need to finish every step before the reveal.

[← Lab Home](../README.md) · [Next: Task 1.2 →](task1-2.md) · [📎 Cheatsheet](cheatsheet.md)

**Need help?** [Jump to the expandable hints](#-stuck-open-a-hint-click-to-expand). Open one at a time.

## Mission

The GitHub Actions workflow successfully compiles a C program. It reports success. But what does that green check actually prove?

Your goal is to make CI verify the **behavior** of the program, not just whether GCC can compile it.

- Inspect `.github/workflows/ci.yaml` and identify what the `test` job currently checks.
- Add a functional check that runs the compiled program with **base `2`** and **exponent `10`**.
- Require the exact output: `2 ^ 10 = 1024`.
- Push your change, examine the result, and investigate any failure.
- Correct the underlying problem and run the workflow again.

**Do not change `expo/expo.c` before running your new test.** The first failure is part of the evidence.

## Evidence to collect

Ask yourself:

- Does the compiler report an error, or does the compiled application return an unexpected answer?
- Which step produces the unexpected result?
- Which file determines the calculation?
- How do you know the test can catch incorrect behavior?

In the Actions log, record the actual output and compare it with the expected output.

## Reference desk

### A. Run an executable in a workflow

A `run:` step executes shell commands on the runner. A program compiled earlier in **the same job** can be executed by its relative path:

```yaml
- name: Run an example program
  run: ./calculator 4 3
```

### B. Capture and compare command output

Bash can capture a command's printed output using `$(...)`. The `test` command returns a nonzero exit status when a comparison is false; GitHub Actions treats a failing step as a failure.

```bash
message=$(printf 'ready')
echo "Observed: $message"
test "$message" = "ready"
```

Combine these two ideas to test **your** compiled executable. Check the compilation step to discover its output filename.

### C. Mathematical clue

The `^` in the program's printed text represents exponentiation. For example, `2` raised to `10` is `1024`. In C, multiplication and exponentiation are not the same operation.

## 🧩 Stuck? Open a hint (click to expand)

<details>
<summary>Hint 1 — Where should the new step go?</summary>

Put it **after compilation**, inside the existing `test` job. Steps in a job run in order and share a workspace.

</details>

<details>
<summary>Hint 2 — The test is red. Is that necessarily bad?</summary>

Not yet. If GCC succeeded but the comparison failed, the new test may have uncovered a real bug. Print the program's actual output and inspect `expo/expo.c`.

</details>

<details>
<summary>Hint 3 — Which C function is relevant?</summary>

Look at the expression passed to `printf`. The C math library provides `pow(base, exponent)`; the compile command already links the math library with `-lm`.

</details>

## Completion criteria

- [ ] The workflow compiles the C application.
- [ ] A functional test executes it with `2` and `10`.
- [ ] The test checks the **actual output**, not just the program's exit code.
- [ ] The initial failure was investigated and the application fixed.
- [ ] The latest GitHub Actions run is green for the right reason.

---

[← Lab Home](../README.md) · [Next: Task 1.2 — The Unprotected Docker Build →](task1-2.md)
