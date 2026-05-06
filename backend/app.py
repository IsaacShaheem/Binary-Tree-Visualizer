from __future__ import annotations

from pathlib import Path
from typing import Any

from flask import Flask, jsonify, request

from .tree_service import TreeService


def create_app(service: TreeService | None = None) -> Flask:
    project_root = Path(__file__).resolve().parents[1]
    app = Flask(
        __name__,
        static_folder=str(project_root / "frontend"),
        static_url_path="",
    )
    app.config["TREE_SERVICES"] = {
        "avl": service or TreeService(kind="avl"),
        "bst": TreeService(kind="bst"),
    }

    def tree_service_from_request() -> TreeService:
        tree_type = request.args.get("type", "avl")
        services: dict[str, TreeService] = app.config["TREE_SERVICES"]
        if tree_type not in services:
            raise ValueError("type must be 'bst' or 'avl'")
        return services[tree_type]

    @app.get("/")
    def index() -> Any:
        return app.send_static_file("index.html")

    @app.get("/api/tree")
    def get_tree() -> Any:
        try:
            return jsonify(tree_service_from_request().current_tree())
        except ValueError as error:
            return jsonify({"error": str(error)}), 400

    @app.post("/api/insert")
    def insert() -> Any:
        payload = request.get_json(silent=True) or {}
        value = payload.get("value")

        if not isinstance(value, int):
            return jsonify({"error": "value must be an integer"}), 400

        try:
            return jsonify(tree_service_from_request().insert(value))
        except ValueError as error:
            return jsonify({"error": str(error)}), 400

    @app.post("/api/delete")
    def delete() -> Any:
        payload = request.get_json(silent=True) or {}
        value = payload.get("value")

        if not isinstance(value, int):
            return jsonify({"error": "value must be an integer"}), 400

        try:
            return jsonify(tree_service_from_request().delete(value))
        except ValueError as error:
            return jsonify({"error": str(error)}), 400

    @app.post("/api/reset")
    def reset() -> Any:
        try:
            return jsonify(tree_service_from_request().reset())
        except ValueError as error:
            return jsonify({"error": str(error)}), 400

    return app


app = create_app()


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8000, debug=True)
