import json
import subprocess
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlparse


ROOT_DIR = Path(__file__).resolve().parent
PROJECT_DIR = ROOT_DIR.parent
SCRIPTS_DIR = PROJECT_DIR / "scripts"
INDEX_FILE = ROOT_DIR / "index.html"

ALLOWED_SCRIPTS = {
    "mode_offline.bat",
    "mode_cloud.bat",
    "relay_on.bat",
    "relay_off.bat",
    "light_on.bat",
    "light_off.bat",
    "cloud_on.bat",
    "cloud_off.bat",
    "status_monitor.bat",
}


class Handler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        # Keep console cleaner for student demo usage.
        return

    def _send_json(self, status_code: int, data: dict):
        body = json.dumps(data).encode("utf-8")
        self.send_response(status_code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        try:
            self.wfile.write(body)
        except (BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
            # Client closed connection before response finished.
            return

    def do_GET(self):
        parsed = urlparse(self.path)
        if parsed.path in ("/", "/index.html"):
            html = INDEX_FILE.read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(html)))
            self.end_headers()
            try:
                self.wfile.write(html)
            except (BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
                # Browser disconnected while loading page.
                return
            return

        self._send_json(404, {"ok": False, "error": "Not found"})

    def do_POST(self):
        parsed = urlparse(self.path)
        if parsed.path != "/run":
            self._send_json(404, {"ok": False, "error": "Not found"})
            return

        params = parse_qs(parsed.query)
        script = (params.get("script", [""])[0]).strip()

        if script not in ALLOWED_SCRIPTS:
            self._send_json(400, {"ok": False, "error": "Script not allowed"})
            return

        script_path = SCRIPTS_DIR / script
        if not script_path.exists():
            self._send_json(404, {"ok": False, "error": "Script file missing"})
            return

        proc = subprocess.run(
            ["cmd", "/c", str(script_path)],
            cwd=str(SCRIPTS_DIR),
            capture_output=True,
            text=True,
            timeout=20,
        )
        try:
            self._send_json(
                200,
                {
                    "ok": proc.returncode == 0,
                    "script": script,
                    "exitCode": proc.returncode,
                    "stdout": proc.stdout[-2000:],
                    "stderr": proc.stderr[-2000:],
                },
            )
        except (BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
            return

    def handle_one_request(self):
        try:
            super().handle_one_request()
        except (BrokenPipeError, ConnectionAbortedError, ConnectionResetError):
            return
        except subprocess.TimeoutExpired:
            self._send_json(200, {"ok": False, "error": "Script execution timed out"})
        except Exception as exc:
            self._send_json(200, {"ok": False, "error": f"Server error: {exc}"})


def main():
    server = ThreadingHTTPServer(("127.0.0.1", 8765), Handler)
    print("UI server running at http://127.0.0.1:8765")
    server.serve_forever()


if __name__ == "__main__":
    main()
