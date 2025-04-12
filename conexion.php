<?php
header("Content-Type: application/json");

class Cconexion {
    private $server = "localhost";
    private $user = "root";
    private $pass = "*choc1999";
    private $db = "enfproyecto";
    private $conexion;

    public function __construct() {
        $this->conexion = new mysqli($this->server, $this->user, $this->pass, $this->db);
        
        if ($this->conexion->connect_errno) {
            die(json_encode(["success" => false, "error" => "Conexión fallida: " . $this->conexion->connect_error]));
        }
    }

    public function registrarUsuario($datos) {
        $stmt = $this->conexion->prepare("INSERT INTO usuarios (nombre_completo, edad, genero, departamento, municipio, peso, altura, actividad_fisica, ocupacion, imc, estado_imc, tmb, calorias_recomendadas) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        
        $stmt->bind_param("sisssddsssdds", 
            $datos->nombreCompleto,
            $datos->edad,
            $datos->genero,
            $datos->departamento,
            $datos->municipio,
            $datos->peso,
            $datos->altura,
            $datos->actividadFisica,
            $datos->ocupacion,
            $datos->imc,
            $datos->estadoIMC,
            $datos->tmb,
            $datos->caloriasRecomendadas
        );
        
        if ($stmt->execute()) {
            $id_usuario = $stmt->insert_id;
            $stmt->close();
            return $id_usuario;
        } else {
            $stmt->close();
            return false;
        }
    }

    public function registrarAlimentos($id_usuario, $alimentos) {
        $stmt = $this->conexion->prepare("INSERT INTO alimentos (id_usuario, nombre_producto, cantidad, calorias, carbohidratos, proteinas, grasas) VALUES (?, ?, ?, ?, ?, ?, ?)");
        
        $registros_exitosos = 0;
        
        foreach ($alimentos as $alimento) {
            $stmt->bind_param("isddddd", 
                $id_usuario,
                $alimento->nombreProducto,
                $alimento->cantidad,
                $alimento->calorias,
                $alimento->carbohidratos,
                $alimento->proteinas,
                $alimento->grasas
            );
            
            if ($stmt->execute()) {
                $registros_exitosos++;
            }
        }
        
        $stmt->close();
        return $registros_exitosos;
    }
}

// Manejo de solicitudes
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $conexion = new Cconexion();
    $datos = json_decode(file_get_contents('php://input'));
    
    if (isset($datos->usuario)) {
        $id_usuario = $conexion->registrarUsuario($datos->usuario);
        
        if ($id_usuario !== false && isset($datos->alimentos) && count($datos->alimentos) > 0) {
            $alimentos_registrados = $conexion->registrarAlimentos($id_usuario, $datos->alimentos);
            echo json_encode([
                "success" => true,
                "message" => "Registro completo",
                "id_usuario" => $id_usuario,
                "alimentos_registrados" => $alimentos_registrados
            ]);
        } elseif ($id_usuario !== false) {
            echo json_encode([
                "success" => true,
                "message" => "Usuario registrado sin alimentos",
                "id_usuario" => $id_usuario
            ]);
        } else {
            echo json_encode([
                "success" => false,
                "error" => "Error al registrar usuario"
            ]);
        }
    } else {
        echo json_encode([
            "success" => false,
            "error" => "Datos incompletos"
        ]);
    }
} else {
    echo json_encode([
        "success" => false,
        "error" => "Método no permitido"
    ]);
}
?>