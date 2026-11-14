<?php
header('Content-Type: application/json; charset=utf-8');

$host = getenv('DB_HOST') ?: 'localhost';
$database = getenv('DB_NAME');
$username = getenv('DB_USER');
$password = getenv('DB_PASSWORD');
if ($database === false || $database === '' ||
    $username === false || $username === '' || $password === false) {
    http_response_code(500);
    echo json_encode(["error" => "Database configuration is missing"]);
    exit;
}

try {
    $conn = new mysqli($host, $username, $password, $database);
    $conn->set_charset("utf8mb4");

    $node = $_GET['node'] ?? 'node_1';

    if (!is_string($node) || !preg_match('/^node_[0-9]+$/', $node)) {
        http_response_code(400);
        echo json_encode(["error" => "Invalid node"]);
        exit;
    }
    header('Cache-Control: no-store');
    if ($node === 'node_6') {
        $conn->query("SET time_zone = '+00:00'");
        $topic = 'testtopic/temp/outTopic/EE470pacgeo8266/potentiometer';
        $stmt = $conn->prepare(
            "SELECT id, value,
             DATE_FORMAT(time_received, '%Y-%m-%dT%H:%i:%sZ') AS time_received
             FROM potentiometer_readings WHERE topic = ? ORDER BY id DESC LIMIT 300"
        );
        $stmt->bind_param("s", $topic);
    } else {
        $stmt = $conn->prepare(
            "SELECT node_name, time_received, temperature
             FROM sensor_data WHERE node_name = ? ORDER BY time_received"
        );
        $stmt->bind_param("s", $node);
    }
    $stmt->execute();

    $result = $stmt->get_result();
    $data = [];

    while ($row = $result->fetch_assoc()) {
        $data[] = $row;
    }

    if ($node === 'node_6') {
        $data = array_reverse($data);
    }
    echo json_encode($data);

    $stmt->close();
    $conn->close();

} catch (Exception $e) {
    http_response_code(500);
    echo json_encode(["error" => "Unable to retrieve sensor data"]);
}
?>