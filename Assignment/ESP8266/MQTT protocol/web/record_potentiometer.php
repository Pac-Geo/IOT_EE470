<?php
header('Content-Type: application/json; charset=utf-8');

function respond(int $status, array $body): void
{
    http_response_code($status);
    echo json_encode($body);
    exit;
}

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    header('Allow: POST');
    respond(405, ['error' => 'POST is required.']);
}

$config = require __DIR__ . '/db_config.php';
$providedToken = $_SERVER['HTTP_X_RELAY_TOKEN'] ?? '';
$expectedToken = $config['relay_token'] ?? '';
if (!is_string($providedToken) || $expectedToken === '' ||
    !hash_equals($expectedToken, $providedToken)) {
    respond(401, ['error' => 'Unauthorized relay.']);
}

$input = json_decode(file_get_contents('php://input'), true);
$value = $input['value'] ?? null;
if (!is_int($value) || $value < 0 || $value > 1023) {
    respond(400, ['error' => 'Value must be an integer from 0 to 1023.']);
}

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
try {
    $connection = new mysqli(
        $config['host'],
        $config['username'],
        $config['password'],
        $config['database']
    );
    $connection->set_charset('utf8mb4');

    $statement = $connection->prepare(
        'INSERT INTO potentiometer_data (adc_value) VALUES (?)'
    );
    $statement->bind_param('i', $value);
    $statement->execute();

    $statement->close();
    $connection->close();

    respond(201, ['saved' => true]);
} catch (mysqli_sql_exception $error) {
    error_log('Potentiometer insert failed.');
    respond(500, ['error' => 'Could not save the reading.']);
}
?>
