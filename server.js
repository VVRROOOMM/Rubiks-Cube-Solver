const cors = require('cors');

const express = require('express');
const app = express();
app.use(cors());

const net = require('net');

const PORT = 5000;
const SERVER_PORT = 5001;

let obj;

const totals = new Array(10).fill(0);
const oldTotals = new Array(10).fill(0);
let total = 0;

let nodeGoals = new Array(10).fill(0);
let goalTotal = 0;

let resetGoal = false;

let t_start = performance.now();
let t_curr = t_start;
let t_old = t_start;
let t_diff = 0;

let speedFlag = 0;

let averageSpeed = new Array(10).fill(0);
let lastSpeed = new Array(10).fill(0);

let cpuUsage = new Array(10).fill(0);
let ramUsage = new Array(10).fill(0);

let resultObj = {};

const server = net.createServer((socket) => {
	console.log('Connection created');

	socket.on('data', (data) => {
		t_old = t_curr;
		t_curr = performance.now();
		t_diff = (t_curr - t_old) / 1000;

		lastSpeed.fill(-1);
		cpuUsage.fill(-1);
		ramUsage.fill(-1);

		resultObj = {};

		if (t_old != t_start) {
			speedFlag = 1;
		}
		else {
			speedFlag = 0;
		}

		//console.log('Received: ', data.toString());
		obj = JSON.parse(data.toString());

		//console.log('Report has been parsed');
		//console.log('Type:' + obj.type);

		if (obj.type == 'report') {
			console.clear();
			console.log('Node Array:');

			for (let i = 0; i < obj.nodes.length; i++) {
				//console.log(i + ': ' + JSON.stringify(obj.nodes[i]));
				let index = obj.nodes[i].nodeID;
				totals[index] += obj.nodes[i].cubesSolved;
				total += obj.nodes[i].cubesSolved;

				cpuUsage[index] = obj.nodes[i].cpuUsage;
				ramUsage[index] = obj.nodes[i].ramUsage;

				if (speedFlag == 1) {
					averageSpeed[index] = totals[index]/((t_curr - t_start) / 1000);
					lastSpeed[index] = (totals[index] - oldTotals[index])/((t_curr - t_old) / 1000);
				}
			}

			console.log('Totals:');

			for (let i = 0; i < totals.length; i++) {
				console.log('node ' + i + ': ' + totals[i] + ' / ' + nodeGoals[i]);
				oldTotals[i] = totals[i];
			}

			console.log('Total Cubes: ' + total + ' / ' + goalTotal);

			resetGoal = true;

			if (speedFlag == 1) {
				for (let i = 0; i < obj.nodes.length; i++) {
					let index = obj.nodes[i].nodeID;
					
					console.log('node ' + index + ':');
					console.log('averageSpeed: ' + averageSpeed[i]);
					console.log('lastSpeed: ' + lastSpeed[i] + '\n');
				}
			}

			createResultObj(resultObj);
		}
		else if (obj.type == 'goals') {
			for (let i = 0; i < obj.nodes.length; i++) {
				nodeGoals[i] = obj.nodes[i].cubesToSolve;
				goalTotal += nodeGoals[i];
			}

			resetGoal = false;
		}
	});

	socket.on('end', () => {
		resultObj = {};
		console.log('Connection ended');
		total = 0;
		totals.fill(0);

		if (resetGoal) {
			goalTotal = 0;
		}

		t_start = performance.now();
		t_curr = t_start;
		t_old = t_start;
	});
});

server.listen(PORT, '127.0.0.1', () => {
	console.log('Server listening to 127.0.0.1:' + PORT);
});

function createResultObj(result) {
	result.totals = totals;
	result.total = total;
	result.nodeGoals = nodeGoals;
	result.averageSpeed = averageSpeed;
	result.lastSpeed = lastSpeed;
	result.cpuUsage = cpuUsage;
	result.ramUsage = ramUsage;

	//console.log(JSON.stringify(result));
}

app.listen(SERVER_PORT, "0.0.0.0", () => {
	console.log('Express is running');
});

app.get('/test', (req, res) => {
	res.send('Hello World!');
});

app.get('/stats', (req, res) => {
	res.send(resultObj);
});