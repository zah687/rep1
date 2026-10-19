# Task 2.2 — A Green Build, a Broken Website

**Classroom timing:** 9 minutes to investigate, then an instructor-led solution reveal. You do not need to finish every step before the reveal.

[← Task 2.1](task2-1.md) · [Lab Home](../README.md) · [Next: Task 2.3 →](task2-3.md) · [📎 Cheatsheet](cheatsheet.md)

**Need help?** [Jump to the expandable hints](#-stuck-open-a-hint-click-to-expand). Open one at a time.

## Mission

The website builds. GitHub Actions shows green. But no one has asked the running server a single question.

Your goal is to turn **an HTTP response into a functional test**. The website has a deliberately incorrect message; your new test should catch it **before** you repair it.

### Part A — Start the website

In `.github/workflows/ci.yaml`, add a step **after** `Build website image` that starts the image `sdo-web:1.0` as a background container.

Your container must:

- Have a name so you can inspect its logs later (`sdo-web`).
- Map runner port `8080` to container port `8080`.
- Keep running while the next GitHub Actions step makes a request.

### Part B — Ask the server a question

Add a step that requests `http://localhost:8080/` using `curl`.

The response contains a message **and a changing visitor counter**. Check only the stable text:

```text
Hello from my C web server!
```

**First push the new test without editing `MESSAGE`.** A red test is expected: that's your evidence that CI can catch a broken website.

### Part C — Follow the evidence

Inspect the failed Actions step. Compare the observed HTTP body with the expected message, find the `MESSAGE` constant in `website/server.c`, fix it, and push again.

## Evidence to collect

- Did Docker build successfully even when the HTTP assertion failed?
- Did the container start? What did `curl` actually receive?
- Was the failure caused by an unreachable port, an HTTP error, or incorrect response text?
- Which source line controls the response message?
- Does the corrected website now pass the same assertion?

## Reference desk

### A. Run a background container

```bash
docker run -d --name sdo-web -p 8080:8080 sdo-web:1.0
```

`-d` runs in the background, `--name` gives the container a stable name, and `-p HOST:CONTAINER` publishes a port. Without `-p`, the runner's `localhost:8080` would not reach this container.

### B. Make an HTTP request

```bash
curl --fail --silent --show-error http://localhost:8080/
```

`--fail` makes HTTP error statuses fail the command, `--silent` suppresses the progress meter, and `--show-error` still prints an error message.

### C. Capture and check a response

The same Bash pattern you used for Expo works with an HTTP response:

```bash
response=$(curl --fail --silent --show-error http://localhost:8080/)
printf '%s\n' "$response"
printf '%s\n' "$response" | grep -Fq 'EXPECTED MESSAGE'
```

`grep -Fq` looks for an exact **substring**, without interpreting it as a regular expression. Replace the placeholder with the expected greeting above.

**Why not compare the whole response?** The visitor counter changes after each request. A whole-body comparison would fail for a healthy website.

### D. One important timing detail

`docker run -d` starts the container, but the server might not be listening by the time the next step executes. If `curl` reports a connection error, inspect `docker ps -a` and `docker logs sdo-web` first. Task 2.3 will make startup checks reliable.

## 🧩 Stuck? Open a hint (click to expand)

<details>
<summary>Hint 1 — The request cannot connect</summary>

Check the port mapping and whether the container is still running. `EXPOSE` in the Dockerfile does not replace `docker run -p`.

</details>

<details>
<summary>Hint 2 — The request succeeds, but the test is red</summary>

Print the response body. Compare its greeting with the required string. A response that arrives successfully can still contain the wrong content.

</details>

<details>
<summary>Hint 3 — Where is the deliberate bug?</summary>

Search for `MESSAGE` near the top of `website/server.c`. Fix only the text; the socket implementation does not need changes.

</details>

## Completion criteria

- [ ] The workflow starts the Docker image as a background container.
- [ ] A `curl` step makes a real HTTP request to the published port.
- [ ] The assertion checks the expected greeting, not only an HTTP status.
- [ ] You observed the deliberately failing test **before** correcting `MESSAGE`.
- [ ] The corrected server passes the HTTP check.

---

[← Task 2.1](task2-1.md) · [Lab Home](../README.md) · **[Next: Task 2.3 — Reliable Checks →](task2-3.md)**
