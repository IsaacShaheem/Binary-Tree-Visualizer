console.log("Visualizer loaded");
let snapshots = [];
let currentLayout = {};
let animationDuration = 1000;
let index = 0;
const svg = document.getElementById("canvas");

function loadSnapshots(fileName){
  fetch(fileName)
    .then(response => response.json())
    .then(data => {
      snapshots = data.filter(
        snap=>Object.keys(snap).length !== 0
      );
      index = 0;
      render()
    });
}

function render(){
  const nextLayout = {};
  calculatePositions(snapshots[index],400,60,0,nextLayout);
  
  animateTransition(currentLayout,nextLayout);
  currentLayout = nextLayout;
}

function drawNode(x,y,key,color){

  const nodeRadius = 20;

  const circle = document.createElementNS("http://www.w3.org/2000/svg","circle");
  const text = document.createElementNS("http://www.w3.org/2000/svg","text");

  circle.setAttribute("cx",x);
  circle.setAttribute("cy",y);
  circle.setAttribute("r",nodeRadius);
  circle.setAttribute("fill",color);
  circle.setAttribute("stroke","black");

  svg.appendChild(circle);

  text.setAttribute("x",x);
  text.setAttribute("y",y+5);
  text.setAttribute("text-anchor","middle");
  text.setAttribute("font-size","14px");
  text.textContent = key;

  svg.appendChild(text);

}

function drawLine(x1,y1,x2,y2){

  const line = document.createElementNS("http://www.w3.org/2000/svg","line");
  
  line.setAttribute("x1",x1);
  line.setAttribute("x2",x2);
  line.setAttribute("y1",y1);
  line.setAttribute("y2",y2);
  line.setAttribute("stroke","black");

  svg.appendChild(line);

}

function drawEdges(node,layout){
  if(!node) return;

  if(node.left){
    drawLine(layout[node.key].x,layout[node.key].y,layout[node.left.key].x,layout[node.left.key].y);
    drawEdges(node.left,layout);
  }

  if(node.right){
    drawLine(layout[node.key].x,layout[node.key].y,layout[node.right.key].x,layout[node.right.key].y);
    drawEdges(node.right,layout);
  }
}

function calculatePositions(node,x,y,level,map){
  if(!node) return;
  const verticalSpace = 80;
  const horizontalSpace = 200/Math.pow(2,level);

  map[node.key] = {x,y};

  calculatePositions(node.left,x - horizontalSpace, y + verticalSpace, level + 1, map);
  calculatePositions(node.right,x + horizontalSpace, y + verticalSpace, level + 1, map);

}

function animateTransition(currentLayout,nextLayout){

  const start = performance.now();
  const rotatedNodes = detectRotations(currentLayout,nextLayout);

  function frame(now){
    const time = Math.min((now - start)/animationDuration,1);
    svg.innerHTML = "";

    for(const key in nextLayout){
      const startPos = currentLayout[key] || nextLayout[key];
      const endPos = nextLayout[key];

      const x = startPos.x + (endPos.x -startPos.x) * time;
      const y = startPos.y + (endPos.y -startPos.y) * time;

      let color = "#cce5ff";
      
      if(time <1){
        if(rotatedNodes.rotatedUp.includes(key)){
          color = "#90ee90";
        }else if(rotatedNodes.rotatedDown.includes(key)){
          color = "#ffcccb";
        }
      }

      drawNode(x,y,key,color);

    }

    drawEdges(snapshots[index],nextLayout);

    if(time < 1) requestAnimationFrame(frame);
  }

  requestAnimationFrame(frame);
}

function detectRotations(currentLayout,nextLayout){

  const rotatedUp = [];
  const rotatedDown = [];

  for(const key in nextLayout){
    if(!currentLayout[key]) continue;

    if(nextLayout[key].y > currentLayout[key].y){
      rotatedUp.push(key);
    }else if(nextLayout[key].y < currentLayout[key].y){
      rotatedDown.push(key);
    }
  }
  return {rotatedUp,rotatedDown};
}

document.getElementById("prev").onclick = () => {
  if(index > 0){
    index--;
    render();
  }
};

document.getElementById("next").onclick = () => {
  if(index < snapshots.length - 1){
    index++
    render();
  }
};

document.getElementById("insert").onclick = () => {
  console.log("Insert clicked");
  const value = document.getElementById("input").value;

  if(value === ""){
    alert("Enter a valid value");
    return;
  }

  fetch(`/insert?value=${value}`)
    .then(response => response.json())
    .then(data => {
      snapshots = data.filter(
        snap => Object.keys(snap).length !== 0
      );
      index = snapshots.length-1;
      render();
    })
    .catch(err => {
      console.error("Insert failed:",err);
    });
};

loadSnapshots("avl_snapshots.json");


