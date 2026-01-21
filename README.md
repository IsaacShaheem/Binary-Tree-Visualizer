# AVL & BST Tree Visualizer

An interactive, full-stack **Binary Search Tree (BST)** and **AVL Tree** visualizer designed to help students and developers understand how tree structures evolve during insertions and rotations.

Unlike many visualizers that simulate logic in JavaScript, this project implements **all tree operations in C**, providing an accurate systems-level view of how BSTs and AVL trees work internally. Structural changes are exported as JSON snapshots and animated smoothly in the browser.

---

## 🌟 Key Features

- Interactive node insertion via the browser
- Visualizes both **BST** and **AVL** trees
- Step-by-step navigation using **Previous / Next**
- Smooth animated AVL rotations
- Color-coded rotation transitions
- Persistent backend tree state
- SQL-based operation logging
- SVG rendering for clarity and performance

---

## 🧠 Motivation

Tree data structures are often difficult to understand because their behavior is dynamic and recursive. This project was built to make those internal changes **visible and intuitive**, especially for AVL trees where rotations can be conceptually challenging.

The goal was not just to “make it work,” but to:
- Understand AVL balancing deeply
- Implement real rotations, not visual tricks
- Bridge low-level systems programming with modern web visualization
- Build an interview-ready systems project

---

## 🏗️ Architecture Overview

This project is split into clearly defined layers:

### 1. Core Logic (C)
- BST and AVL tree implementations
- Height and balance factor maintenance
- All rotations (LL, RR, LR, RL)
- Exports structural snapshots as JSON

### 2. Backend Server (Python)
- HTTP server using `http.server`
- Manages persistent tree state
- Invokes the compiled C binary
- Stores operations and snapshots in SQLite

### 3. Frontend (JavaScript + SVG)
- Fetches tree snapshots from the backend
- Computes layout positions
- Animates transitions using `requestAnimationFrame`
- Renders nodes and edges using SVG

---

## 🧰 Tech Stack

| Layer | Technology |
|-----|-----------|
| Data Structures | C |
| Backend Server | Python |
| Frontend Logic | JavaScript |
| Visualization | SVG |
| Database | SQLite |
| UI | HTML, CSS |
| Build System | Makefile |

---

---

## 🔄 How It Works

1. User enters a value in the browser
2. JavaScript sends an `/insert` request to the server
3. Python server calls the C AVL program
4. C generates updated tree snapshots (JSON)
5. JavaScript animates transitions between snapshots
6. SQL logs the operation and snapshot

---

## ▶️ Running the Project

### Build the C code
```bash
make


