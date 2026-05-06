from __future__ import annotations

from threading import Lock
from typing import Any

from .c_tree import TreeEngine


class TreeService:
    def __init__(self, engine: TreeEngine | None = None, kind: str = "avl") -> None:
        self._engine = engine or TreeEngine(kind=kind)
        self._operations: list[dict[str, Any]] = []
        self._lock = Lock()

    def _response(self, snapshot: dict[str, Any] | None, **fields: Any) -> dict[str, Any]:
        return {
            "tree": snapshot,
            "operations": list(self._operations),
            "meta": self._engine.metadata(),
            **fields,
        }

    def insert(self, value: int) -> dict[str, Any]:
        with self._lock:
            inserted = self._engine.insert(value)
            snapshot = self._engine.export()
            operation = {
                "operation": "insert",
                "value": value,
                "inserted": inserted,
                "snapshot": snapshot,
                "meta": self._engine.metadata(),
            }
            self._operations.append(operation)
            return self._response(snapshot, inserted=inserted)

    def delete(self, value: int) -> dict[str, Any]:
        with self._lock:
            deleted = self._engine.delete(value)
            snapshot = self._engine.export()
            operation = {
                "operation": "delete",
                "value": value,
                "deleted": deleted,
                "snapshot": snapshot,
                "meta": self._engine.metadata(),
            }
            self._operations.append(operation)
            return self._response(snapshot, deleted=deleted)

    def reset(self) -> dict[str, Any]:
        with self._lock:
            self._engine.reset()
            self._operations.clear()
            return self._response(self._engine.export())

    def current_tree(self) -> dict[str, Any]:
        with self._lock:
            return self._response(self._engine.export())

    def close(self) -> None:
        with self._lock:
            self._engine.close()
