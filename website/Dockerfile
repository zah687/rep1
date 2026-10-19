# Website Dockerfile — Task 2.1

FROM gcc:latest

WORKDIR /app

COPY server.c .

RUN gcc -o server server.c

# Documents that the server listens on port 8080 inside the container.
# (It does NOT open the port on your laptop - that is what "-p" does.)
EXPOSE 8080

CMD ["./server"]
