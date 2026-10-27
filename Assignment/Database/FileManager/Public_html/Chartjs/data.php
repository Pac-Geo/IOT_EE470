<?php
header('Content-Type: application/json; charset=utf-8');

$host = "localhost";
$database = "u854844498_GeovaniPalomec";
$username = "u854844498_db_GeovaniP";
$password = "@L379d391";

try {
    $conn = new mysqli($host, $username, $password, $database);
    $conn->set_charset("utf8mb4");

    $node = $_GET['node'] ?? 'node_1';

    $stmt = $conn->prepare(
        "SELECT node_name, time_received, temperature
         FROM sensor_data
         WHERE node_name = ?
         ORDER BY time_received"
    );

    $stmt->bind_param("s", $node);
    $stmt->execute();

    $result = $stmt->get_result();
    $data = [];

    while ($row = $result->fetch_assoc()) {
        $data[] = $row;
    }

    echo json_encode($data);

    $stmt->close();
    $conn->close();

} catch (Exception $e) {
    http_response_code(500);
    echo json_encode(["error" => "Unable to retrieve sensor data"]);
}
?>