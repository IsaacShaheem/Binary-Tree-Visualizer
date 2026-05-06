const homeScreen = document.getElementById("home-screen");
const visualizerScreen = document.getElementById("visualizer-screen");
const singleView = document.getElementById("single-view");
const compareView = document.getElementById("compare-view");
const modeLabel = document.getElementById("mode-label");
const input = document.getElementById("input");
const statusText = document.getElementById("status");
const operationsList = document.getElementById("operations");
const statsPanel = document.getElementById("stats-panel");

const SVG_NS = "http://www.w3.org/2000/svg";
const ANIM_MS = 420;
const STEP_DELAY_MS = 500;
const NODE_RADIUS = 22;
const DEMO_VALUES = [10, 20, 30, 40, 50];

let mode = null;
let operations = [];
let busy = false;
let replayPaused = false;
let replayToken = 0;
let latestTrees = { bst: null, avl: null };

class TreeRenderer {
  constructor(svg) {
    this.svg = svg;
    this.edgeLayer = createSvgElement("g", { class: "edge-layer" });
    this.nodeLayer = createSvgElement("g", { class: "node-layer" });
    this.svg.appendChild(this.edgeLayer);
    this.svg.appendChild(this.nodeLayer);

    this.currentLayout = new Map();
    this.nodeIdsByValue = new Map();
    this.nodeElements = new Map();
    this.edgeElements = new Map();
    this.activeAnimation = 0;
    this.nextNodeId = 1;
  }

  render(tree, options = {}) {
    if (!tree) {
      this.clear();
      return;
    }

    const animationId = ++this.activeAnimation;
    const previousLayout = this.currentLayout;
    const { nodes, edges, layout: nextLayout } = this.calculateLayout(tree);
    const nextNodeIds = new Set(nodes.map((node) => node.id));
    const nextEdgeIds = new Set(edges.map((edge) => edge.id));
    const start = performance.now();

    this.removeStaleElements(nextNodeIds, nextEdgeIds);

    const frame = (now) => {
      if (animationId !== this.activeAnimation) return;

      const progress = Math.min((now - start) / ANIM_MS, 1);
      this.drawFrame(nodes, edges, nextLayout, previousLayout, progress);

      if (progress < 1) {
        requestAnimationFrame(frame);
      } else {
        this.currentLayout = nextLayout;
        if (options.highlight) {
          this.highlightVisibleNodes();
        }
      }
    };

    requestAnimationFrame(frame);
  }

  clear() {
    this.activeAnimation += 1;
    this.currentLayout = new Map();
    this.nodeIdsByValue.clear();
    this.nodeElements.clear();
    this.edgeElements.clear();
    this.nextNodeId = 1;
    this.edgeLayer.replaceChildren();
    this.nodeLayer.replaceChildren();
  }

  highlightVisibleNodes() {
    for (const elements of this.nodeElements.values()) {
      elements.circle.classList.add("rotating");
      window.setTimeout(() => elements.circle.classList.remove("rotating"), 420);
    }
  }

  calculateLayout(root) {
    const layout = new Map();
    const edges = [];

    const visit = (node, depth, minX, maxX, parentId = null) => {
      if (!node) return;

      const id = this.getStableNodeId(node);
      const x = (minX + maxX) / 2;
      const y = 54 + depth * 88;

      layout.set(id, { id, node, x, y, parentId });

      if (parentId) {
        edges.push({ id: `${parentId}->${id}`, from: parentId, to: id });
      }

      visit(node.left, depth + 1, minX, x, id);
      visit(node.right, depth + 1, x, maxX, id);
    };

    visit(root, 0, 40, 860);
    return { nodes: [...layout.values()], edges, layout };
  }

  getStableNodeId(node) {
    if (!this.nodeIdsByValue.has(node.key)) {
      this.nodeIdsByValue.set(node.key, `tree-node-${this.nextNodeId}`);
      this.nextNodeId += 1;
    }

    return this.nodeIdsByValue.get(node.key);
  }

  ensureNodeElement(nodeLayout) {
    if (this.nodeElements.has(nodeLayout.id)) {
      return this.nodeElements.get(nodeLayout.id);
    }

    const group = createSvgElement("g", { class: "node", "data-node-id": nodeLayout.id });
    const circle = createSvgElement("circle", {
      r: NODE_RADIUS,
      class: "node-circle",
    });
    const key = createSvgElement("text", { class: "node-key" });
    const meta = createSvgElement("text", { class: "node-meta" });

    group.appendChild(circle);
    group.appendChild(key);
    group.appendChild(meta);
    this.nodeLayer.appendChild(group);

    const record = { group, circle, key, meta };
    this.nodeElements.set(nodeLayout.id, record);
    return record;
  }

  ensureEdgeElement(edge) {
    if (this.edgeElements.has(edge.id)) {
      return this.edgeElements.get(edge.id);
    }

    const line = createSvgElement("line", { class: "edge", "data-edge-id": edge.id });
    this.edgeLayer.appendChild(line);
    this.edgeElements.set(edge.id, line);
    return line;
  }

  removeStaleElements(nextNodeIds, nextEdgeIds) {
    for (const [id, elements] of this.nodeElements.entries()) {
      if (!nextNodeIds.has(id)) {
        elements.group.remove();
        this.nodeElements.delete(id);
      }
    }

    for (const [id, line] of this.edgeElements.entries()) {
      if (!nextEdgeIds.has(id)) {
        line.remove();
        this.edgeElements.delete(id);
      }
    }
  }

  drawFrame(nodes, edges, nextLayout, previousLayout, progress) {
    const eased = easeInOutCubic(progress);
    const framePositions = new Map();

    for (const nodeLayout of nodes) {
      const start = previousLayout.get(nodeLayout.id) || nextLayout.get(nodeLayout.id);
      const x = interpolate(start.x, nodeLayout.x, eased);
      const y = interpolate(start.y, nodeLayout.y, eased);
      framePositions.set(nodeLayout.id, { x, y });
    }

    for (const edge of edges) {
      const line = this.ensureEdgeElement(edge);
      updateEdgeElement(line, framePositions.get(edge.from), framePositions.get(edge.to));
    }

    for (const nodeLayout of nodes) {
      const elements = this.ensureNodeElement(nodeLayout);
      const position = framePositions.get(nodeLayout.id);
      const color = progress < 1 ? movementColor(nodeLayout.id, nextLayout, previousLayout) : "#dbeafe";
      updateNodeElement(elements, nodeLayout, position.x, position.y, color);
    }
  }
}

const singleRenderer = new TreeRenderer(document.getElementById("canvas"));
const bstRenderer = new TreeRenderer(document.getElementById("bst-canvas"));
const avlRenderer = new TreeRenderer(document.getElementById("avl-canvas"));

function setStatus(message) {
  statusText.textContent = message;
}

async function apiRequest(path, options = {}) {
  const response = await fetch(path, {
    headers: { "Content-Type": "application/json" },
    ...options,
  });

  const payload = await response.json();
  if (!response.ok) {
    throw new Error(payload.error || "Request failed");
  }

  return payload;
}

function createSvgElement(tagName, attributes = {}) {
  const element = document.createElementNS(SVG_NS, tagName);
  setAttributes(element, attributes);
  return element;
}

function setAttributes(element, attributes) {
  for (const [name, value] of Object.entries(attributes)) {
    element.setAttribute(name, value);
  }
}

function interpolate(start, end, progress) {
  return start + (end - start) * progress;
}

function easeInOutCubic(t) {
  return t < 0.5 ? 4 * t * t * t : 1 - Math.pow(-2 * t + 2, 3) / 2;
}

function delay(ms) {
  return new Promise((resolve) => window.setTimeout(resolve, ms));
}

async function waitWhilePaused(token) {
  while (replayPaused && replayToken === token) {
    await delay(120);
  }
}

function movementColor(id, nextLayout, previousLayout) {
  const previous = previousLayout.get(id);
  const next = nextLayout.get(id);

  if (!previous) return "#dbeafe";
  if (next.y < previous.y) return "#bbf7d0";
  if (next.y > previous.y) return "#fecaca";
  return "#dbeafe";
}

function updateNodeElement(elements, nodeLayout, x, y, color) {
  setAttributes(elements.circle, {
    cx: x,
    cy: y,
    fill: color,
  });

  setAttributes(elements.key, {
    x,
    y: y + 5,
  });
  elements.key.textContent = nodeLayout.node.key;

  setAttributes(elements.meta, {
    x,
    y: y + 40,
  });
  elements.meta.textContent =
    nodeLayout.node.height === undefined ? "" : `h:${nodeLayout.node.height} b:${nodeLayout.node.balance}`;
}

function updateEdgeElement(line, from, to) {
  setAttributes(line, {
    x1: from.x,
    y1: from.y,
    x2: to.x,
    y2: to.y,
  });
}

function getHeight(node) {
  if (!node) return -1;
  return 1 + Math.max(getHeight(node.left), getHeight(node.right));
}

function countNodes(node) {
  if (!node) return 0;
  return 1 + countNodes(node.left) + countNodes(node.right);
}

function capitalize(value) {
  return value.charAt(0).toUpperCase() + value.slice(1);
}

function renderStats() {
  const bstHeight = getHeight(latestTrees.bst);
  const avlHeight = getHeight(latestTrees.avl);
  const nodeCount = mode === "compare" ? countNodes(latestTrees.avl) : countNodes(latestTrees[mode]);

  if (mode === "compare") {
    statsPanel.innerHTML = `
      <div class="stat-card"><span class="stat-label">BST Height</span><span class="stat-value">${bstHeight}</span></div>
      <div class="stat-card"><span class="stat-label">AVL Height</span><span class="stat-value">${avlHeight}</span></div>
      <div class="stat-card"><span class="stat-label">Nodes</span><span class="stat-value">${nodeCount}</span></div>
    `;
    return;
  }

  statsPanel.innerHTML = `
    <div class="stat-card"><span class="stat-label">${mode?.toUpperCase()} Height</span><span class="stat-value">${getHeight(latestTrees[mode])}</span></div>
    <div class="stat-card"><span class="stat-label">Nodes</span><span class="stat-value">${nodeCount}</span></div>
    <div class="stat-card"><span class="stat-label">Mode</span><span class="stat-value">${mode?.toUpperCase()}</span></div>
  `;
}

function setControlsDisabled(disabled) {
  input.disabled = disabled;
  document.getElementById("insert").disabled = disabled;
  document.getElementById("delete").disabled = disabled;
  document.getElementById("run-demo").disabled = disabled;
  document.getElementById("reset").disabled = disabled;
  document.getElementById("play-replay").disabled = disabled;
  document.getElementById("change-mode").disabled = disabled;
  document.querySelectorAll(".mode-button").forEach((button) => {
    button.disabled = disabled;
  });
}

function showMode(selectedMode) {
  mode = selectedMode;
  operations = [];
  replayToken += 1;
  replayPaused = false;
  latestTrees = { bst: null, avl: null };
  homeScreen.classList.add("hidden");
  visualizerScreen.classList.remove("hidden");
  singleView.classList.toggle("hidden", mode === "compare");
  compareView.classList.toggle("hidden", mode !== "compare");
  modeLabel.textContent = mode === "compare" ? "BST vs AVL comparison" : `${mode.toUpperCase()} mode`;
  input.value = "";
  setStatus("");
  renderOperations();
  renderStats();
  resetCurrentMode();
}

function showHome() {
  mode = null;
  operations = [];
  replayToken += 1;
  replayPaused = false;
  visualizerScreen.classList.add("hidden");
  homeScreen.classList.remove("hidden");
  clearRenderers();
  latestTrees = { bst: null, avl: null };
  setStatus("");
}

function clearRenderers() {
  singleRenderer.clear();
  bstRenderer.clear();
  avlRenderer.clear();
}

function selectedTypes() {
  return mode === "compare" ? ["bst", "avl"] : [mode];
}

function apiPath(endpoint, treeType) {
  return `/api/${endpoint}?type=${treeType}`;
}

async function mutateTree(endpoint, value) {
  const body = JSON.stringify({ value });
  const requests = selectedTypes().map((treeType) =>
    apiRequest(apiPath(endpoint, treeType), { method: "POST", body }).then((payload) => {
      if (endpoint === "insert" && payload.inserted === false) {
        payload.meta = { ...(payload.meta || {}), duplicate: true };
      }
      return { treeType, payload };
    }),
  );
  return Promise.all(requests);
}

async function resetCurrentMode() {
  if (!mode) return;

  const requests = selectedTypes().map((treeType) =>
    apiRequest(apiPath("reset", treeType), { method: "POST" }).then((payload) => ({
      treeType,
      payload,
    })),
  );
  applyResults(await Promise.all(requests));
}

function applyResults(results) {
  for (const { treeType, payload } of results) {
    latestTrees[treeType] = payload.tree || null;
  }

  if (mode === "compare") {
    const bst = results.find((result) => result.treeType === "bst")?.payload;
    const avl = results.find((result) => result.treeType === "avl")?.payload;
    const rotation = avl?.meta?.rotation;
    requestAnimationFrame(() => {
      bstRenderer.render(bst?.tree || null);
      avlRenderer.render(avl?.tree || null, { highlight: Boolean(rotation) });
    });
  } else {
    const result = results[0];
    const rotation = result?.payload.meta?.rotation;
    singleRenderer.render(result?.payload.tree || null, {
      highlight: result?.treeType === "avl" && Boolean(rotation),
    });
  }

  renderStats();
}

async function applyOp(action, value, options = {}) {
  const results = await mutateTree(action, value);
  applyResults(results);

  if (options.record !== false) {
    operations.push({ type: action, value });
    recordOperation(action, value, results);
  }

  return results;
}

async function safeApply(action, value, options = {}) {
  if (busy) return null;

  busy = true;
  setControlsDisabled(true);
  try {
    const results = await applyOp(action, value, options);
    if (options.waitForAnimation !== false) {
      await delay(ANIM_MS);
    }
    return results;
  } finally {
    busy = false;
    setControlsDisabled(false);
  }
}

function recordOperation(action, value, results) {
  const resultText = results
    .map(({ treeType, payload }) => {
      if (action === "insert") {
        return `${treeType.toUpperCase()}: ${payload.inserted ? "inserted" : "duplicate"}`;
      }
      return `${treeType.toUpperCase()}: ${payload.deleted ? "deleted" : "not found"}`;
    })
    .join(", ");

  renderOperations(`${action} ${value} (${resultText})`);
}

function renderOperations(latestLabel = null) {
  operationsList.innerHTML = "";
  const labels = operations
    .slice()
    .reverse()
    .map((operation) => `${operation.type} ${operation.value}`);

  if (latestLabel) {
    labels[0] = latestLabel;
  }

  for (const label of labels) {
    const item = document.createElement("li");
    item.textContent = label;
    operationsList.appendChild(item);
  }
}

function readIntegerInput() {
  const value = Number(input.value);
  if (!Number.isInteger(value)) {
    setStatus("Enter an integer value.");
    return null;
  }
  return value;
}

async function handleInsert() {
  const value = readIntegerInput();
  if (value === null || !mode || busy) return;

  try {
    const results = await safeApply("insert", value);
    if (!results) return;
    input.value = "";
    setStatus(results.some(({ payload }) => payload.meta?.duplicate) ? "Value already exists" : `Inserted ${value}.`);
  } catch (error) {
    setStatus(error.message);
  }
}

async function handleDelete() {
  const value = readIntegerInput();
  if (value === null || !mode || busy) return;

  try {
    const results = await safeApply("delete", value);
    if (!results) return;
    input.value = "";
    setStatus(results.some(({ payload }) => payload.deleted) ? `Deleted ${value}.` : "Value not found");
  } catch (error) {
    setStatus(error.message);
  }
}

async function replayOperations() {
  if (!mode || busy || operations.length === 0) return;

  const token = ++replayToken;
  const replayList = operations.slice();
  replayPaused = false;

  try {
    busy = true;
    setControlsDisabled(true);
    setStatus("Replaying operations...");
    await resetCurrentMode();

    for (const [index, operation] of replayList.entries()) {
      if (token !== replayToken) return;
      await waitWhilePaused(token);
      if (token !== replayToken) return;
      setStatus(`Step ${index + 1}: ${capitalize(operation.type)} ${operation.value}`);
      await applyOp(operation.type, operation.value, { record: false });
      await delay(STEP_DELAY_MS);
    }

    setStatus("Replay complete.");
  } catch (error) {
    setStatus(error.message);
  } finally {
    busy = false;
    setControlsDisabled(false);
    replayPaused = false;
  }
}

async function runDemo() {
  if (!mode || busy) return;

  try {
    busy = true;
    setControlsDisabled(true);
    setStatus("Running demo...");
    operations = [];
    renderOperations();
    await resetCurrentMode();

    for (const value of DEMO_VALUES) {
      await applyOp("insert", value);
      await delay(STEP_DELAY_MS);
    }

    setStatus("Demo complete.");
  } catch (error) {
    setStatus(error.message);
  } finally {
    busy = false;
    setControlsDisabled(false);
  }
}

document.querySelectorAll(".mode-button").forEach((button) => {
  button.addEventListener("click", () => showMode(button.dataset.mode));
});

document.getElementById("change-mode").addEventListener("click", showHome);
document.getElementById("insert").addEventListener("click", handleInsert);
document.getElementById("delete").addEventListener("click", handleDelete);
document.getElementById("run-demo").addEventListener("click", runDemo);
document.getElementById("play-replay").addEventListener("click", replayOperations);
document.getElementById("pause-replay").addEventListener("click", () => {
  replayPaused = !replayPaused;
  setStatus(replayPaused ? "Replay paused." : "Replay resumed.");
});
document.getElementById("reset-replay").addEventListener("click", async () => {
  replayToken += 1;
  replayPaused = false;
  await resetCurrentMode();
  setStatus("Replay reset to an empty tree.");
});
document.getElementById("reset").addEventListener("click", async () => {
  if (!mode || busy) return;

  try {
    busy = true;
    setControlsDisabled(true);
    operations = [];
    renderOperations();
    await resetCurrentMode();
    await delay(ANIM_MS);
    setStatus("Tree reset.");
  } catch (error) {
    setStatus(error.message);
  } finally {
    busy = false;
    setControlsDisabled(false);
  }
});

input.addEventListener("keydown", (event) => {
  if (event.key === "Enter") {
    handleInsert();
  }
});
