const net = require('net');

const PORT = 5000;

let reportObj;

const server = net.createServer((socket) => {
	console.log('Connection created');

	socket.on('data', (data) => {
		console.log('Received: ', data.toString());
		reportObj = JSON.parse(data.toString());

		console.log('Report has been parsed');
		console.log('Type:' + reportObj.type);
		console.log('Node Array:');

		for (let i = 0; i < reportObj.nodes.length; i++) {
			console.log(i + ': ' + JSON.stringify(reportObj.nodes[i]));
		}
	});

	socket.on('end', () => {
		console.log('Connection ended');
	});
});

server.listen(PORT, '127.0.0.1', () => {
	console.log('Server listening to 127.0.0.1:' + PORT);
});