$(document).ready(function () {
    const node = new URLSearchParams(window.location.search).get("node") || "node_1";
    const isPot = node === "node_6";
    const status = document.getElementById("chart-status");
    let chart;

    function refresh() {
        $.ajax({
            url: "data.php",
            method: "GET",
            dataType: "json",
            cache: false,
            data: { node: node },
            success: function (data) {
                if (!Array.isArray(data)) {
                    status.textContent = "Unable to retrieve readings.";
                    return;
                }
                const labels = data.map(row => isPot
                    ? new Date(row.time_received).toLocaleString()
                    : row.time_received);
                const values = data.map(row => Number(isPot ? row.value : row.temperature));
                if (chart) {
                    chart.data.labels = labels;
                    chart.data.datasets[0].data = values;
                    chart.update("none");
                } else {
                    chart = new Chart(document.getElementById("mycanvas"), {
                        type: "line",
                        data: {
                            labels: labels,
                            datasets: [{
                                label: isPot ? "Potentiometer (ADC)" : "Temperature (°C)",
                                backgroundColor: "rgba(0,128,0,0.20)",
                                borderColor: "rgba(0,128,0,1)",
                                pointBackgroundColor: "rgba(0,128,0,1)",
                                data: values
                            }]
                        },
                        options: {
                            responsive: true,
                            animation: false,
                            plugins: {
                                title: {
                                    display: true,
                                    text: isPot ? "Node 6 - Potentiometer" :
                                        "Sensor " + node.replace("_", " ").replace("node", "Node")
                                }
                            },
                            scales: {
                                x: {
                                    title: { display: true, text: isPot ? "Time (local)" : "Time" },
                                    ticks: { maxTicksLimit: 12 }
                                },
                                y: isPot ? {
                                    min: 0, max: 1023,
                                    title: { display: true, text: "Potentiometer (ADC, 0–1023)" }
                                } : {
                                    title: { display: true, text: "Temperature (°C)" }
                                }
                            }
                        }
                    });
                }
                status.textContent = data.length ? (isPot
                    ? "Latest 300 stored readings. Timezone: " +
                      Intl.DateTimeFormat().resolvedOptions().timeZone
                    : "") : "No readings stored yet.";
            },
            error: function () {
                status.textContent = "Unable to load readings. Check the PHP file and database table.";
            },
            complete: function () {
                // The parent dashboard already refreshes its iframe every 5 seconds.
                // A directly opened Node 6 graph also refreshes by itself.
                if (isPot && window.self === window.top) setTimeout(refresh, 5000);
            }
        });
    }
    refresh();
});
