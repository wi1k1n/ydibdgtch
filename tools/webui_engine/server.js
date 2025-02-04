const express = require('express');
const { spawn } = require('child_process');
const WebSocket = require('ws');

const args = process.argv.slice(2); // Remove first two elements (node and script path)

if (args.length === 0) {
    console.error("Usage: node server.js <appPath> [appArgs...]");
    process.exit(1);
}

const appPath = args[0]; // First argument is the executable path
const appArgs = args.slice(1); // Remaining arguments are passed to the app

const app = express();
const PORT = 3000;

// Serve frontend files
app.use(express.static(__dirname));

const server = app.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
    console.log(`Spawning process: ${appPath} ${appArgs.join(" ")}`);
});

const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
    console.log('Client connected');

    // Start the C++ process
    const cppProcess = spawn(appPath, appArgs);

    cppProcess.stdout.on('data', (data) => {
        ws.send(data.toString()); // Send CLI output to frontend
    });

    cppProcess.stderr.on('data', (data) => {
        ws.send(`Error: ${data}`);
    });

    cppProcess.on('close', (code) => {
        ws.send(`Process exited with code ${code}`);
    });

    // Receive messages from frontend and send them to C++ process
    ws.on('message', (message) => {
        cppProcess.stdin.write(message + '\n'); // Send input to C++ CLI
    });

    ws.on('close', () => {
        console.log('Client disconnected');
        cppProcess.kill(); // Stop C++ process when client disconnects
    });
});
