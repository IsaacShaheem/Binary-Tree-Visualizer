from __future__ import annotations

import ctypes
import json
import platform
from pathlib import Path
from typing import Any


class TreeLibraryError(RuntimeError):
    pass


class TreeEngine:
    def __init__(self, kind: str = "avl", library_path: Path | None = None) -> None:
        if kind not in {"avl", "bst"}:
            raise ValueError("kind must be 'avl' or 'bst'")

        self.kind = kind
        self._lib = ctypes.CDLL(str(library_path or self._default_library_path()))
        self._configure_signatures()
        self._tree = self._create_tree()

    @staticmethod
    def _default_library_path() -> Path:
        extension = "dylib" if platform.system() == "Darwin" else "so"
        return Path(__file__).resolve().parents[1] / "build" / f"libtree.{extension}"

    def _configure_signatures(self) -> None:
        self._lib.tree_free_json.argtypes = [ctypes.c_void_p]
        self._lib.tree_free_json.restype = None

        for prefix in ("avl", "bst"):
            getattr(self._lib, f"{prefix}_create").argtypes = []
            getattr(self._lib, f"{prefix}_create").restype = ctypes.c_void_p

            getattr(self._lib, f"{prefix}_insert").argtypes = [ctypes.c_void_p, ctypes.c_int]
            getattr(self._lib, f"{prefix}_insert").restype = ctypes.c_bool

            getattr(self._lib, f"{prefix}_search").argtypes = [ctypes.c_void_p, ctypes.c_int]
            getattr(self._lib, f"{prefix}_search").restype = ctypes.c_bool

            getattr(self._lib, f"{prefix}_delete").argtypes = [ctypes.c_void_p, ctypes.c_int]
            getattr(self._lib, f"{prefix}_delete").restype = ctypes.c_bool

            getattr(self._lib, f"{prefix}_export_json").argtypes = [ctypes.c_void_p]
            getattr(self._lib, f"{prefix}_export_json").restype = ctypes.c_void_p

            getattr(self._lib, f"{prefix}_destroy").argtypes = [ctypes.c_void_p]
            getattr(self._lib, f"{prefix}_destroy").restype = None

        self._lib.avl_last_rotation.argtypes = [ctypes.c_void_p]
        self._lib.avl_last_rotation.restype = ctypes.c_char_p

    def _create_tree(self) -> ctypes.c_void_p:
        tree = getattr(self._lib, f"{self.kind}_create")()
        if not tree:
            raise TreeLibraryError("C tree allocation failed")
        return tree

    def insert(self, value: int) -> bool:
        self._ensure_open()
        return bool(getattr(self._lib, f"{self.kind}_insert")(self._tree, int(value)))

    def search(self, value: int) -> bool:
        self._ensure_open()
        return bool(getattr(self._lib, f"{self.kind}_search")(self._tree, int(value)))

    def delete(self, value: int) -> bool:
        self._ensure_open()
        return bool(getattr(self._lib, f"{self.kind}_delete")(self._tree, int(value)))

    def metadata(self) -> dict[str, Any]:
        self._ensure_open()
        if self.kind != "avl":
            return {"rotation": None}

        rotation = self._lib.avl_last_rotation(self._tree).decode("utf-8")
        return {"rotation": rotation or None}

    def export(self) -> dict[str, Any] | None:
        self._ensure_open()
        export = getattr(self._lib, f"{self.kind}_export_json")
        json_pointer = export(self._tree)
        if not json_pointer:
            raise TreeLibraryError("C JSON export failed")

        try:
            raw_json = ctypes.string_at(json_pointer).decode("utf-8")
            return json.loads(raw_json)
        finally:
            self._lib.tree_free_json(json_pointer)

    def reset(self) -> None:
        self.close()
        self._tree = self._create_tree()

    def close(self) -> None:
        if getattr(self, "_tree", None):
            getattr(self._lib, f"{self.kind}_destroy")(self._tree)
            self._tree = None

    def _ensure_open(self) -> None:
        if not getattr(self, "_tree", None):
            raise TreeLibraryError("TreeEngine is closed")

    def __enter__(self) -> TreeEngine:
        return self

    def __exit__(self, *_exc: object) -> None:
        self.close()

    def __del__(self) -> None:
        try:
            self.close()
        except Exception:
            pass
