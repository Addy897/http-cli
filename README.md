# http-cli

A crossplatform  curl-like HTTP client written in C++ from scratch. Raw sockets for HTTP, OpenSSL for HTTPS. No libcurl.

```bash
./main https://example.com
./main -x POST -H "Content-Type:application/json" -d '{"key":"val"}' https://echo.free.beeceptor.com/
./main -i https://example.com          # print response headers
./main -p https://example.com          # strip tags, print plain text
./main file:///path/to/file.html
./main view-source:https://example.com
```

## Build

**Linux**

```bash
apt install libssl-dev zlib1g-dev libjsoncpp-dev
make
```

**Windows (MinGW)**

```bash
pacman -S mingw-w64-x86_64-openssl mingw-w64-x86_64-zlib mingw-w64-x86_64-jsoncpp
make
```
To build as a static library for embedding in other projects:

```bash
make lib   # produces build/libhttp.a
```

## Flags

| flag | what it does |
|---|---|
| `-x METHOD` | GET (default), POST, HEAD |
| `-H key:value` | add a request header |
| `-d data` | request body for POST |
| `-i` | include response headers in output |
| `-p` | strip HTML tags from response body |
| `-l LEVEL` | logging: DEBUG, INFO, WARN, ERROR, ALL |
| `-h` | usage |

## What's implemented

**HTTP/1.1** with keep-alive. Connections are pooled by URL — if you make two requests to the same host the socket is reused. If the server closes it between requests, the EPIPE on the next write is caught and it reconnects automatically.

**Redirects** — follows 301 and 302, handles both absolute and relative `Location` headers, stops after 5 hops.

**Chunked transfer encoding** — fully handled, including partial reads and buffer overlap between chunks.

**Gzip decompression** via zlib. The `Decoder` class keeps a persistent `z_stream` across calls so chunked gzip works correctly — decompressing each chunk independently would break because gzip state carries over between chunks.

**Caching** — respects `Cache-Control: max-age=N` and `no-store`. Singleton `CacheStore` with TTL expiry, doesn't persist across processes.

**URL schemes** — `http://`, `https://`, `file://`, `view-source:`. The `file://` scheme reads the local file and returns a fake 200 response so the rest of the pipeline doesn't need a special case.

## Related

Network layer of a browser stack I'm building from scratch. [html-renderer](https://github.com/Addy897/html-renderer) handles parsing and displaying the HTML.
