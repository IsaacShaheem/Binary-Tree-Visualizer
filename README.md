# Binary Tree Visualizer

An interactive full-stack visualizer for Binary Search Trees and AVL Trees. It was built to make tree operations, balancing behavior and AVL rotations easier to understand through real-time SVG animation.

The core data structure logic is written in C, exposed to Python through `ctypes`, served by a Flask API and rendered in the browser with vanilla JavaScript.

## Features

- Visualize Binary Search Tree and AVL Tree behavior
- Compare BST vs AVL side by side
- Insert and delete nodes interactively
- Smooth SVG animations for node movement and rotations
- Live statistics for height and node count
- Replay operation history step by step
- Demo mode that shows how AVL trees stay balanced
- Responsive, minimal browser UI

## Architecture

```text
C (BST/AVL logic)
  ↓
Python ctypes wrapper
  ↓
Flask API
  ↓
Frontend SVG visualizer
```

This architecture keeps each layer focused. C owns the tree algorithms and memory management. Python uses `ctypes` to call the compiled shared library directly, avoiding subprocess overhead and file-based communication. Flask handles request routing and JSON responses while the frontend focuses on rendering, animation and interaction.

## Screenshots / Demo

![Demo Screenshot](images/demo.png)

Demo GIF or video:

![Demo GIF](images/demo.gif)

## How AVL Trees Work

A regular BST can become unbalanced if values are inserted in sorted order. In the worst case it starts behaving like a linked list, which makes search, insert and delete slower.

An AVL tree keeps itself balanced by checking height differences after updates. When a subtree becomes too unbalanced, the tree performs rotations: LL, RR, LR or RL. Compare mode makes this visible by showing the same operations applied to both a BST and an AVL tree.

## Tech Stack

- C
- Python
- ctypes
- Flask
- JavaScript
- SVG
- HTML/CSS

## Project Structure

```text
c/
  tree.c        BST and AVL implementation
  tree.h        Public C API for the shared library

backend/
  c_tree.py     ctypes wrapper around the C library
  tree_service.py
                Tree state and operation coordination
  app.py        Flask API and static frontend serving

frontend/
  index.html    App layout
  style.css     UI styling
  visualize.js  SVG rendering, animation, replay and demo logic

tests/
  test_tree.c   C tests for BST and AVL behavior
  test_c_tree.py
                ctypes integration tests
  test_app.py   Flask API tests
```

## Installation & Running

Install Python dependencies:

```bash
python3 -m pip install -r requirements.txt
```

Build the C shared library:

```bash
make
```

Start the Flask app:

```bash
python3 -m backend.app
```

Open:

```text
http://127.0.0.1:8000
```

## Running Tests

Run C tests:

```bash
make test
```

Run Python integration and API tests:

```bash
python3 -m unittest tests/test_c_tree.py tests/test_app.py
```

## Interesting Technical Details

- AVL insert and delete support LL, RR, LR and RL rebalancing cases
- C tree nodes are allocated and freed explicitly to avoid leaks
- Python calls the compiled C shared library directly through `ctypes`
- Flask exposes a small JSON API for insert, delete, reset and tree state
- Compare mode synchronizes BST and AVL SVG rendering in the same animation frame
- Replay rebuilds tree state from recorded operations with controlled timing

## Future Improvements

- Persistent save and load for operation histories
- Additional structures such as Red-Black Trees or Heaps
- More mobile layout refinement for small screens

## Final Notes

This project connects systems programming, data structures and full-stack development in one small application. It shows how low-level C logic can power an interactive browser experience while keeping the architecture clean and testable.
