/*
 * server.c - a tiny web server written in C.
 *
 * You do NOT need to understand the networking code below.
 * Your only job: change the MESSAGE text, then build and run it with Docker.
 */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080   /* the port the server listens on INSIDE the container */

/* ===== TODO: change this message (plain text - no HTML needed) ===== */
const char *MESSAGE = "Hello from my broken C web server!";
/* =================================================================== */

/* Lets "docker stop" and Ctrl+C shut the server down immediately */
static void stop_server(int sig) {
    (void)sig;
    _exit(0);
}

int main(void) {
    signal(SIGTERM, stop_server);
    signal(SIGINT, stop_server);

    /* Create a network socket and listen on PORT */
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0 ||
        listen(server, 10) < 0) {
        perror("Could not start server");
        return 1;
    }

    printf("Server is listening on port %d inside the container...\n", PORT);
    fflush(stdout);

    int visitors = 0;
    while (1) {
        /* Wait for a browser to connect */
        int client = accept(server, NULL, NULL);
        if (client < 0) {
            continue;
        }

        /* Read the browser's request */
        char request[2048] = {0};
        if (read(client, request, sizeof(request) - 1) <= 0) {
            close(client);
            continue;
        }

        /* Browsers also ask for a small icon - we don't have one */
        if (strncmp(request, "GET /favicon.ico", 16) == 0) {
            const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                    "Content-Length: 0\r\n"
                                    "Connection: close\r\n\r\n";
            send(client, not_found, strlen(not_found), 0);
            close(client);
            continue;
        }

        /* Build the answer: your message + a visitor counter */
        visitors++;
        char body[1024];
        snprintf(body, sizeof(body), "%s\n\nYou are visitor number %d.\n",
                 MESSAGE, visitors);

        char response[2048];
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain; charset=utf-8\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n\r\n%s",
                 strlen(body), body);
        send(client, response, strlen(response), 0);
        close(client);

        printf("Answered visitor number %d\n", visitors);
        fflush(stdout);
    }
}
