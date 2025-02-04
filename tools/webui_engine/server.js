const APPPATH = "D:\\prj\\chess_maxon\\dgt_chess_board\\ydibdgtch\\_build\\Debug\\YDIBDGTCH.exe";
const APPARGS = ["engineweb"];

const express = require('express');
const { spawn } = require('child_process');
const WebSocket = require('ws');

const app = express();
const PORT = 3000;

// Serve frontend files
app.use(express.static(__dirname));

const server = app.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
});

const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
    console.log('Client connected');

    // Start the C++ process
    const cppProcess = spawn(APPPATH, APPARGS);

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
