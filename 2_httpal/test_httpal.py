#!/usr/bin/env python3
"""
Test harness for httpal.

For each test it binds a fake HTTP server on an ephemeral loopback port, runs
the httpal binary against it, captures the *exact* bytes httpal put on the
wire, sends back a well-formed canned response, and then checks both the
request httpal built and the body httpal printed.

Everything is plain http:// on 127.0.0.1, so no TLS and no real network.

Usage:
    python3 test_httpal.py [path-to-httpal] [-v]

    -v / --verbose   dump the raw request bytes for every test, not just failures
"""

import socket
import subprocess
import sys
import threading

BIN = "./httpal"
VERBOSE = False

RESPONSE_BODY = b"HTTPAL_TEST_BODY_OK"
RESPONSE = (
    b"HTTP/1.1 200 OK\r\n"
    b"Content-Type: text/plain\r\n"
    b"Content-Length: " + str(len(RESPONSE_BODY)).encode() + b"\r\n"
    b"\r\n" + RESPONSE_BODY
)

CONNECT_TIMEOUT = 5.0   # how long to wait for httpal to connect
READ_TIMEOUT = 3.0      # how long to wait for the request headers
DRAIN_TIMEOUT = 0.4     # extra sweep to catch trailing bytes of malformed requests
RUN_TIMEOUT = 8.0       # hard limit on the httpal process


# --------------------------------------------------------------------------
# results
# --------------------------------------------------------------------------

class Results:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.failures = []

    def check(self, name, ok, detail=""):
        if ok:
            self.passed += 1
            print(f"  \033[32mPASS\033[0m  {name}")
        else:
            self.failed += 1
            self.failures.append(name)
            print(f"  \033[31mFAIL\033[0m  {name}")
            if detail:
                for line in str(detail).splitlines():
                    print(f"        {line}")


R = Results()


def section(title):
    print(f"\n\033[1m{title}\033[0m")


# --------------------------------------------------------------------------
# fake server
# --------------------------------------------------------------------------

class FakeServer:
    """
    One-shot HTTP server on an ephemeral port.

    Captures the complete request byte stream. It does not rely on the client
    closing its write side (httpal never does), and it does not rely purely on
    a timeout either: it reads until the header terminator, then reads
    Content-Length more bytes, then does a short drain sweep so that malformed
    requests (e.g. a premature blank line that pushes real headers into the
    "body") are still captured in full rather than truncated.
    """

    def __init__(self, response=RESPONSE):
        self.response = response
        self.request = b""
        self.connected = False
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._sock.bind(("127.0.0.1", 0))
        self._sock.listen(1)
        self._sock.settimeout(CONNECT_TIMEOUT)
        self.port = self._sock.getsockname()[1]
        self._thread = threading.Thread(target=self._serve, daemon=True)

    def __enter__(self):
        self._thread.start()
        return self

    def __exit__(self, *exc):
        self._thread.join(timeout=CONNECT_TIMEOUT + READ_TIMEOUT + 2)
        try:
            self._sock.close()
        except OSError:
            pass
        return False

    def _serve(self):
        try:
            conn, _ = self._sock.accept()
        except (socket.timeout, OSError):
            return
        self.connected = True
        with conn:
            self.request = self._read_request(conn)
            try:
                conn.sendall(self.response)
            except OSError:
                pass
            # closing tells httpal's recv() loop it has hit EOF
            try:
                conn.shutdown(socket.SHUT_WR)
            except OSError:
                pass

    def _read_request(self, conn):
        data = b""
        conn.settimeout(READ_TIMEOUT)

        # 1. read until the first header terminator
        try:
            while b"\r\n\r\n" not in data:
                chunk = conn.recv(4096)
                if not chunk:
                    return data
                data += chunk
        except socket.timeout:
            return data

        # 2. honour Content-Length if the (first) header block declares one
        head, _, body = data.partition(b"\r\n\r\n")
        length = 0
        for line in head.split(b"\r\n")[1:]:
            if line.lower().startswith(b"content-length:"):
                try:
                    length = int(line.split(b":", 1)[1].strip())
                except ValueError:
                    length = 0
        try:
            while len(body) < length:
                chunk = conn.recv(4096)
                if not chunk:
                    break
                body += chunk
        except socket.timeout:
            pass

        data = head + b"\r\n\r\n" + body

        # 3. drain sweep: catches bytes that a malformed request left trailing
        conn.settimeout(DRAIN_TIMEOUT)
        try:
            while True:
                chunk = conn.recv(4096)
                if not chunk:
                    break
                data += chunk
        except socket.timeout:
            pass

        return data


def run(args):
    """Run httpal. Returns (returncode|None, stdout, stderr). None rc means timeout."""
    try:
        p = subprocess.run(
            [BIN] + args,
            capture_output=True,
            text=True,
            timeout=RUN_TIMEOUT,
        )
        return p.returncode, p.stdout, p.stderr
    except subprocess.TimeoutExpired as e:
        out = e.stdout or ""
        err = e.stderr or ""
        if isinstance(out, bytes):
            out = out.decode(errors="replace")
        if isinstance(err, bytes):
            err = err.decode(errors="replace")
        return None, out, err
    except FileNotFoundError:
        print(f"\033[31mCannot run '{BIN}' - does it exist and is it executable?\033[0m")
        sys.exit(2)


def exchange(path_and_flags, scheme="http"):
    """
    Start a server, point httpal at it, return (server, rc, stdout, stderr).

    In path_and_flags the literal token "{URL}" is replaced with the generated
    URL. scheme="https" builds an https:// URL (still plaintext on the wire) to
    exercise scheme stripping.
    """
    with FakeServer() as srv:
        url = f"{scheme}://127.0.0.1:{srv.port}"
        args = [tok.replace("{URL}", url) for tok in path_and_flags]
        rc, out, err = run(args)
    return srv, rc, out, err


# --------------------------------------------------------------------------
# request assertions
# --------------------------------------------------------------------------

def dump(raw, rc=None, out=None, err=None):
    parts = [f"raw request = {raw!r}"]
    if rc is not None or out or err:
        parts.append(f"rc={rc} stdout={(out or '')[:300]!r} stderr={(err or '')[:300]!r}")
    return "\n".join(parts)


def check_request(label, srv, method, path, expect_body=b"",
                  expect_headers=(), rc=None, out=None, err=None):
    """
    Structural validation of one request.

    The key check is the split at the FIRST \\r\\n\\r\\n: everything before it
    must be headers, everything after it must be exactly the expected body.
    A premature blank line therefore shows up as real headers leaking into the
    body, which is precisely the bug class this harness exists to catch.
    """
    raw = srv.request
    d = dump(raw, rc, out, err)

    if not srv.connected:
        R.check(f"{label}: connected to server", False, d)
        return
    R.check(f"{label}: connected to server", True)

    if not raw:
        R.check(f"{label}: sent any bytes", False, d)
        return

    R.check(
        f"{label}: request line is '{method} {path} HTTP/1.1'",
        raw.startswith(f"{method} {path} HTTP/1.1\r\n".encode()),
        d,
    )

    R.check(
        f"{label}: has header terminator (\\r\\n\\r\\n)",
        b"\r\n\r\n" in raw,
        d,
    )
    if b"\r\n\r\n" not in raw:
        return

    head, _, body = raw.partition(b"\r\n\r\n")
    head_lines = head.split(b"\r\n")

    R.check(
        f"{label}: body after separator is exactly the expected payload",
        body == expect_body,
        f"expected body {expect_body!r}\ngot body      {body!r}\n{d}",
    )

    # every line after the request line must look like a header
    bad = [l for l in head_lines[1:] if l and b":" not in l]
    R.check(
        f"{label}: no malformed lines in header block",
        not bad,
        f"offending lines: {bad!r}\n{d}",
    )

    R.check(
        f"{label}: no empty line inside the header block",
        all(l for l in head_lines[1:]),
        d,
    )

    R.check(
        f"{label}: Host header present in header block",
        any(l.lower().startswith(b"host:") for l in head_lines[1:]),
        d,
    )

    R.check(
        f"{label}: 'Connection: close' is in the header block, not the body",
        any(l.lower().startswith(b"connection:") for l in head_lines[1:]),
        d,
    )

    for h in expect_headers:
        R.check(
            f"{label}: custom header {h!r} present in header block",
            any(l == h for l in head_lines[1:]),
            f"header lines = {head_lines[1:]!r}\n{d}",
        )

    # POST/PUT always declare a length (0 is legitimate for an empty payload);
    # GET/DELETE carry no body here, so they should declare none at all.
    if method in ("POST", "PUT"):
        want = f"content-length:{len(expect_body)}".encode()
        R.check(
            f"{label}: Content-Length matches body length ({len(expect_body)})",
            any(l.lower().replace(b" ", b"") == want for l in head_lines[1:]),
            f"header lines = {head_lines[1:]!r}\n{d}",
        )
    else:
        R.check(
            f"{label}: no Content-Length on a request with no body",
            not any(l.lower().startswith(b"content-length:") for l in head_lines[1:]),
            f"header lines = {head_lines[1:]!r}\n{d}",
        )

    if VERBOSE:
        print(f"        {raw!r}")


# --------------------------------------------------------------------------
# tests
# --------------------------------------------------------------------------

def test_url_parsing():
    section("URL parsing")

    srv, rc, out, err = exchange(["{URL}/path"])
    check_request("path '/path'", srv, "GET", "/path", rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/"])
    check_request("trailing slash only", srv, "GET", "/", rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}"])
    check_request("no path at all", srv, "GET", "/", rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/a/b/c"])
    check_request("nested path", srv, "GET", "/a/b/c", rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/q?x=1&y=2"])
    check_request("query string preserved", srv, "GET", "/q?x=1&y=2", rc=rc, out=out, err=err)

    # https:// with an explicit port: exercises scheme stripping and the
    # explicit-port branch of the parser. Still plaintext on the wire.
    srv, rc, out, err = exchange(["{URL}/secure"], scheme="https")
    check_request("https:// scheme stripped, explicit port used",
                  srv, "GET", "/secure", rc=rc, out=out, err=err)


def test_methods():
    section("Methods")

    srv, rc, out, err = exchange(["{URL}/x"])
    check_request("implicit GET (no -X)", srv, "GET", "/x", rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/x", "-X", "GET"])
    check_request("explicit -X GET", srv, "GET", "/x", rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/x", "-X", "DELETE"])
    check_request("-X DELETE", srv, "DELETE", "/x", rc=rc, out=out, err=err)

    data = "hello=world"
    srv, rc, out, err = exchange(["{URL}/x", "-X", "POST", "-d", data])
    check_request("-X POST with body", srv, "POST", "/x",
                  expect_body=data.encode(), rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/x", "-X", "PUT", "-d", data])
    check_request("-X PUT with body", srv, "PUT", "/x",
                  expect_body=data.encode(), rc=rc, out=out, err=err)


def test_headers():
    section("Custom headers (-H)")

    hdr = "X-Test: abc"
    data = '{"a":1}'

    srv, rc, out, err = exchange(["{URL}/x", "-X", "POST", "-d", data, "-H", hdr + "\r\n"])
    check_request("POST with -H", srv, "POST", "/x",
                  expect_body=data.encode(),
                  expect_headers=[hdr.encode()],
                  rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/x", "-X", "PUT", "-d", data, "-H", hdr + "\r\n"])
    check_request("PUT with -H", srv, "PUT", "/x",
                  expect_body=data.encode(),
                  expect_headers=[hdr.encode()],
                  rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/x", "-X", "DELETE", "-H", hdr + "\r\n"])
    check_request("DELETE with -H", srv, "DELETE", "/x",
                  expect_headers=[hdr.encode()],
                  rc=rc, out=out, err=err)

    # a method that ignores -H should still emit a valid request
    srv, rc, out, err = exchange(["{URL}/x", "-X", "GET", "-H", hdr + "\r\n"])
    check_request("GET with -H still well-formed", srv, "GET", "/x",
                  rc=rc, out=out, err=err)


def test_body_edge_cases():
    section("Body edge cases")

    data = "a" * 5000  # larger than the 4096 read buffer
    srv, rc, out, err = exchange(["{URL}/big", "-X", "POST", "-d", data])
    check_request("POST body larger than buffer", srv, "POST", "/big",
                  expect_body=data.encode(), rc=rc, out=out, err=err)

    srv, rc, out, err = exchange(["{URL}/empty", "-X", "POST", "-d", ""])
    check_request("POST with empty body", srv, "POST", "/empty",
                  expect_body=b"", rc=rc, out=out, err=err)


def test_response_handling():
    section("Response handling")

    with FakeServer() as srv:
        rc, out, err = run([f"http://127.0.0.1:{srv.port}/x"])

    R.check(
        "process exits 0 on a successful request",
        rc == 0,
        f"rc={rc} stdout={out!r} stderr={err!r}",
    )
    R.check(
        "response body is printed to stdout",
        RESPONSE_BODY.decode() in out,
        f"stdout={out!r}",
    )
    R.check(
        "response headers are stripped from the printed body",
        "HTTP/1.1 200" not in out and "Content-Type:" not in out,
        f"stdout={out!r}",
    )


def test_large_response():
    section("Large response")

    body = b"X" * 9000
    resp = (
        b"HTTP/1.1 200 OK\r\n"
        b"Content-Length: " + str(len(body)).encode() + b"\r\n"
        b"\r\n" + body
    )
    with FakeServer(response=resp) as srv:
        rc, out, err = run([f"http://127.0.0.1:{srv.port}/big"])

    R.check(
        "multi-chunk response reassembled fully (9000 bytes)",
        out.count("X") == 9000,
        f"got {out.count('X')} X's, rc={rc}, stderr={err[:200]!r}",
    )


def test_errors():
    section("Error handling")

    rc, out, err = run([])
    R.check("missing URL exits nonzero", rc is not None and rc != 0,
            f"rc={rc} stdout={out!r} stderr={err!r}")

    with FakeServer() as srv:
        rc, out, err = run([f"http://127.0.0.1:{srv.port}/x", "-X", "PATCH"])
    R.check("unknown method exits nonzero", rc is not None and rc != 0,
            f"rc={rc} stdout={out!r} stderr={err!r}")
    R.check("unknown method does not hang", rc is not None,
            "process had to be killed by timeout")

    rc, out, err = run(["http://no-such-host-xyzzy.invalid/"])
    R.check("unresolvable host exits nonzero", rc is not None and rc != 0,
            f"rc={rc} stdout={out!r} stderr={err!r}")
    R.check("unresolvable host does not hang", rc is not None,
            "process had to be killed by timeout")

    rc, out, err = run(["-Z", "http://127.0.0.1:1/"])
    R.check("unknown flag exits nonzero", rc is not None and rc != 0,
            f"rc={rc} stdout={out!r} stderr={err!r}")

    # nothing is listening on port 1
    rc, out, err = run(["http://127.0.0.1:1/"])
    R.check("refused connection exits nonzero", rc is not None and rc != 0,
            f"rc={rc} stdout={out!r} stderr={err!r}")


def main():
    global BIN, VERBOSE

    args = list(sys.argv[1:])
    for flag in ("-v", "--verbose"):
        if flag in args:
            VERBOSE = True
            args.remove(flag)
    if args:
        BIN = args[0]

    print(f"Testing binary: {BIN}")

    test_url_parsing()
    test_methods()
    test_headers()
    test_body_edge_cases()
    test_response_handling()
    test_large_response()
    test_errors()

    total = R.passed + R.failed
    print(f"\n\033[1m{R.passed}/{total} passed, {R.failed} failed\033[0m")
    if R.failures:
        print("\nFailed checks:")
        for f in R.failures:
            print(f"  - {f}")
    sys.exit(1 if R.failed else 0)


if __name__ == "__main__":
    main()