<?php
header('Content-Type: application/json; charset=utf-8');

$config = require __DIR__ . '/db_config.php';
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

try {
    $connection = new mysqli(
        $config['host'],
        $config['username'],
        $config['password'],
        $config['database']
    );
    $connection->set_charset('utf8mb4');

    $result = $connection->query(
        'SELECT id, time_received, adc_value
         FROM (
             SELECT id, time_received, value
             FROM potentiometer_data
             ORDER BY id DESC
             LIMIT 200
         ) AS recent_readings
         ORDER BY id ASC'
    );

    $readings = [];
    while ($row = $result->fetch_assoc()) {
        $readings[] = $row;
    }

    echo json_encode($readings);
    $connection->close();
} catch (mysqli_sql_exception $error) {
    http_response_code(500);
    echo json_encode(['error' => 'Could not retrieve potentiometer readings.']);
}
?>
