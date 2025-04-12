<?php
class Cconexion {
    function DBconexion(){
        $server = "localhost";
        $user = "root";
        $pass = "*choc1999";
        $db = "enfproyecto";

        $conexion = new mysqli($server, $user, $pass, $db);

        if ($conexion->connect_errno) {
            die("Conexion Fallida: " . $conexion->connect_error);
        } else {
            echo "conectado con la base de datos";
        }
        return $conexion;
    }
}
include('Cconexion.php');

header("Content-Type: application/json");

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $datos = json_decode(file_get_contents('php://input'), true);
    
    // Validar datos recibidos
    if (empty($datos['nombres']) || empty($datos['apellidos']) || empty($datos['dpi'])) {
        http_response_code(400);
        echo json_encode(["success" => false, "error" => "Datos incompletos"]);
        exit;
    }

    $conexion = (new Cconexion())->DBconexion();
    
    // Consulta preparada para seguridad
    $stmt = $conexion->prepare("INSERT INTO personas (nombres, apellidos, dpi, email, telefono, direccion) VALUES (?, ?, ?, ?, ?, ?)");
    $stmt->bind_param("ssssss", 
        $datos['nombres'],
        $datos['apellidos'],
        $datos['dpi'],
        $datos['email'],
        $datos['telefono'],
        $datos['direccion']
    );
    
    if ($stmt->execute()) {
        echo json_encode(["success" => true, "message" => "Persona registrada", "id" => $conexion->insert_id]);
    } else {
        http_response_code(500);
        echo json_encode(["success" => false, "error" => $conexion->error]);
    }
    
    $stmt->close();
    $conexion->close();
} else {
    http_response_code(405);
    echo json_encode(["success" => false, "error" => "Método no permitido"]);
}
?>