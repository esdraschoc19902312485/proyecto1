#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Tus estructuras Usuario y Alimento permanecen igual...

// Función callback para cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

bool enviarDatosServidor(const Usuario& usuario, const vector<Alimento>& alimentos) {
    CURL* curl;
    CURLcode res;
    string response;

    curl = curl_easy_init();
    if (curl) {
        // Construir el objeto JSON para enviar
        json datos;
        datos["usuario"]["nombreCompleto"] = usuario.nombreCompleto;
        datos["usuario"]["edad"] = usuario.edad;
        datos["usuario"]["genero"] = usuario.genero;
        datos["usuario"]["departamento"] = usuario.departamento;
        datos["usuario"]["municipio"] = usuario.municipio;
        datos["usuario"]["peso"] = usuario.peso;
        datos["usuario"]["altura"] = usuario.altura;
        datos["usuario"]["actividadFisica"] = usuario.actividadFisica;
        datos["usuario"]["ocupacion"] = usuario.ocupacion;
        datos["usuario"]["imc"] = usuario.imc;
        datos["usuario"]["estadoIMC"] = usuario.estadoIMC;
        datos["usuario"]["tmb"] = usuario.tmb;
        datos["usuario"]["caloriasRecomendadas"] = usuario.caloriasRecomendadas;

        for (const auto& alimento : alimentos) {
            json alimentoJson;
            alimentoJson["nombreProducto"] = alimento.nombreProducto;
            alimentoJson["cantidad"] = alimento.cantidad;
            alimentoJson["calorias"] = alimento.calorias;
            alimentoJson["carbohidratos"] = alimento.carbohidratos;
            alimentoJson["proteinas"] = alimento.proteinas;
            alimentoJson["grasas"] = alimento.grasas;
            datos["alimentos"].push_back(alimentoJson);
        }

        string jsonStr = datos.dump();

        // Configurar cURL
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/proyecto/conexion.php");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto respuesta = json::parse(response);
            if (respuesta["success"]) {
                cout << "\n✅ Datos registrados exitosamente en la base de datos." << endl;
                cout << "ID de usuario: " << respuesta["id_usuario"] << endl;
                if (respuesta.contains("alimentos_registrados")) {
                    cout << "Alimentos registrados: " << respuesta["alimentos_registrados"] << endl;
                }
                return true;
            } else {
                cerr << "\n❌ Error al registrar datos: " << respuesta["error"] << endl;
                return false;
            }
        } else {
            cerr << "\n❌ Error en la conexión: " << curl_easy_strerror(res) << endl;
            return false;
        }
    }
    return false;
}

// [Tus funciones existentes permanecen igual...]

int main() {
    Usuario usuario;
    vector<Alimento> alimentos;
    
    registrarUsuario(usuario);
    
    char opcion;
    do {
        ingresarAlimento(alimentos);
        
        cout << "\n¿Desea ingresar otro alimento? (s/n): ";
        cin >> opcion;
        limpiarBuffer();
    } while (toupper(opcion) == 'S');
    
    mostrarResumen(usuario, alimentos);

    // Preguntar si desea guardar en la base de datos
    cout << "\n¿Desea guardar estos datos en la base de datos? (s/n): ";
    cin >> opcion;
    limpiarBuffer();

    if (toupper(opcion) == 'S') {
        if (enviarDatosServidor(usuario, alimentos)) {
            cout << "Datos guardados correctamente." << endl;
        } else {
            cout << "No se pudieron guardar los datos." << endl;
        }
    }
    
    return 0;
}