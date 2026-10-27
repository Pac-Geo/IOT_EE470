$(document).ready(function () {

    const params = new URLSearchParams(window.location.search);
    const node = params.get("node") || "node_1";

    $.ajax({
        url: "data.php",
        method: "GET",
        dataType: "json",
        data: { node: node },

        success: function (data) {

            const x_axis = [];
            const y_axis = [];

            for (const row of data) {
                x_axis.push(row.time_received);
                y_axis.push(Number(row.temperature));
            }

            const chartdata = {
                labels: x_axis,
                datasets: [{
                    label: "Temperature (°C)",
                    backgroundColor: "rgba(0, 128, 0, 0.20)",
                    borderColor: "rgba(0, 128, 0, 1)",
                    pointBackgroundColor: "rgba(0, 128, 0, 1)",
                    hoverBackgroundColor: "rgba(0, 128, 0, 0.75)",
                    hoverBorderColor: "rgba(0, 128, 0, 1)",
                    data: y_axis
                }]
            };

            const ctx = $("#mycanvas");

            new Chart(ctx, {
                type: "line",
                data: chartdata,
                options: {
                    responsive: true,
                    plugins: {
                        title: {
                            display: true,
                            text: "Sensor " + node.replace("_", " ").replace("node", "Node")
                        }
                    },
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: "Time"
                            }
                        },
                        y: {
                            title: {
                                display: true,
                                text: "Temperature (°C)"
                            }
                        }
                    }
                }
            });
        },

        error: function (error) {
            console.log("Error retrieving sensor data:", error);
        }
    });

});