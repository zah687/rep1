
FROM gcc:latest AS build

WORKDIR /app

COPY expo.c .

RUN gcc -static -o sdo-expo expo.c -lm

FROM scratch

COPY --from=build /app/sdo-expo /sdo-expo

ENTRYPOINT ["/app/sdo-expo"]
