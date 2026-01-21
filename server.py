from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs
import subprocess
import json
import sqlite3

PORT = 8000

tree_values = []

connection = sqlite3.connect("visualizer/visualizer.db", check_same_thread=False)
cursor = connection.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS operations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    operation TEXT NOT NULL,
    value INTEGER,
    snapshot TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    
)
""")
connection.commit()

class Handler(BaseHTTPRequestHandler):

    def do_GET(self):
        parsed = urlparse(self.path)

        # -------------------------
        # Serve main HTML
        # -------------------------
        if parsed.path == "/":
            self.serve_file("visualizer/index.html", "text/html")
            return

        # -------------------------
        # Serve JS
        # -------------------------
        if parsed.path == "/visualize.js":
            self.serve_file("visualizer/visualize.js", "application/javascript")
            return

        # -------------------------
        # INSERT operation
        # -------------------------
        if parsed.path == "/insert":
            query = parse_qs(parsed.query)
            value = query.get("value", [None])[0]

            if value is None:
                self.send_error(400, "Missing value")
                return
            
            value = int(value)

            # 1️⃣ Save value in Python (persistent)
            tree_values.append(value)

            # 2️⃣ Call C AVL program with ALL values
            subprocess.run(
                ["./bin/avl", *map(str,tree_values)],
                check=True
            )

            with open("visualizer/avl_snapshots.json") as file:
                snapshot_json = file.read()

            cursor.execute(
                "INSERT INTO operations(operation,value,snapshot) VALUES (?,?,?)",
                ("insert",value,snapshot_json)
            )
        
            connection.commit()

            self.send_response(200)
            self.send_header("Content-Type","application/json")
            self.end_headers()
            self.wfile.write(snapshot_json.encode())
            return
        # -------------------------
        # Fallback
        # -------------------------
        self.send_error(404)

    def serve_file(self, filename, content_type):
        try:
            with open(filename, "rb") as f:
                content = f.read()

            self.send_response(200)
            self.send_header("Content-Type", content_type)
            self.end_headers()
            self.wfile.write(content)

        except FileNotFoundError:
            self.send_error(404)


print(f"Server running at http://localhost:{PORT}")
HTTPServer(("localhost", PORT), Handler).serve_forever()
