function loadStats()
{
	fetch("http://10.0.0.190:5001/stats")
	.then(res => res.json())
	.then (data => {
		if (Object.keys(data).length === 0) {
			return;
		}
		
		const container = document.getElementById("nodes");
		container.innerHTML = "";
		
		const size = data.totals.length;
		
		for (let i = 0; i < size; i++) {
			const box = document.createElement("div");
			box.className = "stat-box";
			
			if (data.cpuUsage[i] === -1 && data.ramUsage[i] === -1 && data.lastSpeed[i] === -1) {
				box.innerHTML = `
					<div class = "box-title">Node ${i}</div>
					<div>Node finished working!</div>
					<div>Average Cubes Per Second: ${data.averageSpeed[i].toFixed(3)}</div>
				`;
			}
			else {
				box.innerHTML = `
					<div class = "box-title">Node ${i}</div>
					<div>Cubes Solved: ${data.totals[i]}</div>
					<div>Total Cubes To Solve: ${data.nodeGoals[i]}</div>
					<div>Average Cubes Per Second: ${data.averageSpeed[i].toFixed(3)}</div>
					<div>Cubes Per Second Since Last Report: ${data.lastSpeed[i].toFixed(3)}</div>
					<div>CPU Usage: ${data.cpuUsage[i].toFixed(3)}%</div>
					<div>RAM Usage: ${data.ramUsage[i].toFixed(3)}%</div>
				`;
			}
			
			container.appendChild(box);
		}
		
		setTimeout(loadStats, 2000);
	})
	.catch (err => {
		console.log('Error: ', err);
	});
}