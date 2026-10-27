</h2>
<?php
// EE 470 - Sensor Dashboard and Data Insertion

$host = "localhost";
$database = "u854844498_GeovaniPalomec";
$username = "u854844498_db_GeovaniP";
$password = "@L379d391";

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

try {
    $conn = new mysqli($host, $username, $password, $database);
    $conn->set_charset("utf8mb4");
} catch (mysqli_sql_exception $e) {
    http_response_code(500);
    exit("Database connection failed.");
}

// Process sensor measurements sent through HTTP GET
if (isset($_GET['node_name']) || isset($_GET['nodeId'])) {

    header("Content-Type: text/plain; charset=utf-8");

    $node = $_GET['node_name'] ?? $_GET['nodeId'];
    $temperature = $_GET['temperature'] ?? $_GET['nodeTemp'] ?? null;
    $humidity = $_GET['humidity'] ?? null;
    $time = $_GET['time_received'] ?? $_GET['timeReceived'] ?? null;

    // Validate node name format
    if (!is_string($node) ||
        !preg_match('/^node_[0-9]+$/', $node)) {
        http_response_code(400);
        exit("REJECTED: Invalid node name.");
    }

    // Validate temperature and humidity
    if (!is_scalar($temperature) ||
        !is_scalar($humidity) ||
        !is_numeric($temperature) ||
        !is_numeric($humidity)) {
        http_response_code(400);
        exit("REJECTED: Temperature and humidity are required.");
    }

    $temperature = (float)$temperature;
    $humidity = (float)$humidity;

    if (!is_finite($temperature) ||
        !is_finite($humidity) ||
        $temperature < -10 || $temperature > 100 ||
        $humidity < 0 || $humidity > 100) {
        http_response_code(400);
        exit("REJECTED: Measurement outside allowed range.");
    }

    // Check that sensor is registered
    $stmt = $conn->prepare(
        "SELECT node_name FROM sensor_register WHERE node_name = ?"
    );
    $stmt->bind_param("s", $node);
    $stmt->execute();
    $registeredNode = $stmt->get_result()->fetch_assoc();
    $stmt->close();

    if (!$registeredNode) {
        http_response_code(400);
        exit("REJECTED: Sensor is not registered.");
    }

    // Validate optional timestamp
    if ($time !== null && $time !== '') {
        if (!is_string($time)) {
            http_response_code(400);
            exit("REJECTED: Invalid timestamp.");
        }

        $date = DateTimeImmutable::createFromFormat(
            '!Y-m-d H:i:s', $time
        );

        if (!$date ||
            $date->format('Y-m-d H:i:s') !== $time) {
            http_response_code(400);
            exit("REJECTED: Use YYYY-MM-DD HH:MM:SS.");
        }
    }

    try {
        // Let MariaDB generate the timestamp if omitted
        if ($time === null || $time === '') {
            $stmt = $conn->prepare(
                "INSERT INTO sensor_data
                 (node_name, time_received, temperature, humidity)
                 VALUES (?, CURRENT_TIMESTAMP, ?, ?)"
            );
            $stmt->bind_param(
                "sdd", $node, $temperature, $humidity
            );
        } else {
            $stmt = $conn->prepare(
                "INSERT INTO sensor_data
                 (node_name, time_received, temperature, humidity)
                 VALUES (?, ?, ?, ?)"
            );
            $stmt->bind_param(
                "ssdd", $node, $time, $temperature, $humidity
            );
        }

        $stmt->execute();
        $stmt->close();

        echo "SUCCESS: Measurement inserted for $node";
        $conn->close();
        exit;

    } catch (mysqli_sql_exception $e) {
        if ($e->getCode() == 1062) {
            http_response_code(409);
            exit("REJECTED: Duplicate timestamp for this sensor.");
        }

        http_response_code(500);
        exit("Database insertion failed.");
    }
}

// Retrieve registered sensors
$registered = $conn->query(
    "SELECT * FROM sensor_register ORDER BY node_name"
);

// Retrieve measurements sorted by node and time
$measurements = $conn->query(
    "SELECT * FROM sensor_data
     ORDER BY node_name, time_received"
);

// Get available nodes for dropdown
$nodes = $conn->query(
    "SELECT node_name FROM sensor_register ORDER BY node_name"
);

$nodeNames = [];
while ($row = $nodes->fetch_assoc()) {
    $nodeNames[] = $row['node_name'];
}

// Select sensor for averages
$selectedNode = $_GET['node'] ?? 'node_1';

if (!is_string($selectedNode) ||
    !in_array($selectedNode, $nodeNames, true)) {
    $selectedNode = $nodeNames[0] ?? '';
}

// Calculate averages
$stmt = $conn->prepare(
    "SELECT
        AVG(temperature) AS avg_temperature,
        AVG(humidity) AS avg_humidity,
        COUNT(*) AS total_readings
     FROM sensor_data
     WHERE node_name = ?"
);

$stmt->bind_param("s", $selectedNode);
$stmt->execute();
$average = $stmt->get_result()->fetch_assoc();
$stmt->close();
?>

<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>EE 470 - Sensor Dashboard</title>

<style>
body {
    font-family: Arial, sans-serif;
    margin: 30px;
}

table {
    border-collapse: collapse;
    margin-bottom: 30px;
}

th, td {
    border: 1px solid black;
    padding: 8px 12px;
    text-align: center;
}

th {
    background-color: lightgreen;
}

.average-heading {
    display: flex;
    align-items: center;
    gap: 18px;
    flex-wrap: wrap;
}

.average-heading h2 {
    margin-bottom: 15px;
}

.sensor-select {
    font-family: Arial, sans-serif;
    font-size: 16px;
    padding: 7px 12px;
    border: 1px solid #888;
    border-radius: 5px;
    background: white;
    cursor: pointer;
}
</style>
</head>


<body>

<h1>EE 470 - Sensor Dashboard</h1>

<h2>Registered Sensor Nodes</h2>

<table>
<tr>
    <th>Node Name</th>
    <th>Manufacturer</th>
    <th>Longitude</th>
    <th>Latitude</th>
</tr>

<?php while ($row = $registered->fetch_assoc()): ?>
<tr>
    <td><?= htmlspecialchars($row['node_name']) ?></td>
    <td><?= htmlspecialchars($row['manufacturer'] ?? '') ?></td>
    <td><?= htmlspecialchars((string)($row['longitude'] ?? '')) ?></td>
    <td><?= htmlspecialchars((string)($row['latitude'] ?? '')) ?></td>
</tr>
<?php endwhile; ?>
</table>

<div class="average-heading">
    <h2>Average Sensor Measurements</h2>

    <form method="GET">
        <select name="node"
                class="sensor-select"
                aria-label="Select sensor"
                onchange="this.form.submit()">

            <?php foreach ($nodeNames as $nodeName): ?>
            <option
                value="<?= htmlspecialchars($nodeName) ?>"
                <?= $selectedNode === $nodeName ? 'selected' : '' ?>>
                <?= htmlspecialchars($nodeName) ?>
            </option>
            <?php endforeach; ?>

        </select>
    </form>
</div>

<table>
<tr>
    <th>Sensor</th>
    <th>Readings</th>
    <th>Average Temperature (°C)</th>
    <th>Average Humidity (%)</th>
</tr>
<tr>
    <td><?= htmlspecialchars($selectedNode) ?></td>
    <td><?= (int)$average['total_readings'] ?></td>
    <td>
        <?= $average['avg_temperature'] !== null
            ? number_format((float)$average['avg_temperature'], 2)
            : 'N/A' ?>
    </td>
    <td>
        <?= $average['avg_humidity'] !== null
            ? number_format((float)$average['avg_humidity'], 2)
            : 'N/A' ?>
    </td>
</tr>
</table>

<h2>Temperature Graph - <?= htmlspecialchars($selectedNode) ?></h2>

<iframe
    src="Chartjs/graph.html?node=<?= urlencode($selectedNode) ?>"
    width="100%"
    height="450"
    style="border: none;"
    title="Sensor Temperature Graph">
</iframe>


<h2>Sensor Data</h2>

<table>
<tr>
    <th>Node Name</th>
    <th>Time Received</th>
    <th>Temperature (°C)</th>
    <th>Humidity (%)</th>
</tr>

<?php while ($row = $measurements->fetch_assoc()): ?>
<tr>
    <td><?= htmlspecialchars($row['node_name']) ?></td>
    <td><?= htmlspecialchars($row['time_received']) ?></td>
    <td><?= htmlspecialchars($row['temperature']) ?></td>
    <td><?= htmlspecialchars($row['humidity']) ?></td>
</tr>
<?php endwhile; ?>
</table>

</body>
</html>

<?php $conn->close(); ?>
