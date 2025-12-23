let events = [];
let threads = {};
let counter = 0;
let animationSpeed = 800;
let isRunning = false;

function startSimulation() {
    if (isRunning) return;

    const logText = document.getElementById("logInput").value.trim();
    if (!logText) {
        alert("Paste simulation log first.");
        return;
    }

    resetSimulation();
    events = logText.split("\n").filter(l => l.trim() !== "");
    isRunning = true;
    document.getElementById("startBtn").disabled = true;
    runStep(0);
}

function resetSimulation() {
    isRunning = false;
    document.getElementById("startBtn").disabled = false;
    document.getElementById("threadContainer").innerHTML = "";
    threads = {};
    counter = 0;
    document.getElementById("counter").innerText = "0";
    events = [];
}

function runStep(index) {
    if (index >= events.length || !isRunning) {
        alert("Simulation Completed");
        isRunning = false;
        document.getElementById("startBtn").disabled = false;
        return;
    }

    const line = events[index].trim();

    if (line.startsWith("Thread")) {
        const parts = line.split(" ");
        const id = parts[1];
        const state = parts[3];

        if (!threads[id]) {
            const div = document.createElement("div");
            div.className = "thread NEW";
            div.id = "thread-" + id;
            div.innerHTML = `Thread ${id}<br><small>NEW</small>`;
            document.getElementById("threadContainer").appendChild(div);
            threads[id] = div;
        }

        threads[id].className = "thread " + state;
        threads[id].innerHTML = `Thread ${id}<br><small>${state}</small>`;
    }

    if (line.startsWith("Final Shared Counter")) {
        counter = line.split(":")[1].trim();
        document.getElementById("counter").innerText = counter;
    }

    animationSpeed = document.getElementById("speedSlider").value;
    setTimeout(() => runStep(index + 1), animationSpeed);
}