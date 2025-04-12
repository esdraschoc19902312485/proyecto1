#include <iostream>
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>
#include <regex>

using json = nlohmann::json;

// Callback para la respuesta HTTP
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// Valida el formato del email
bool validarEmail(const std::string& email) {
    const std::regex patron(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, patron);
}

// Valida el formato del teléfono (ejemplo: 12345678)
bool validarTelefono(const std::string& telefono) {
    const std::regex patron(R"(\d{8,15})");
    return std::regex_match(telefono, patron);
}

bool registrarPersona(const json& datos) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Usar HTTPS para seguridad
        curl_easy_setopt(curl, CURLOPT_URL, "https://tuservidor.com/registro.php");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, datos.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            try {
                auto respuesta = json::parse(response);
                if (respuesta.contains("success") && respuesta["success"]) {
                    std::cout << "\nÉxito: " << respuesta["message"] 
                              << " - ID: " << respuesta["id"] << std::endl;
                    return true;
                } else {
                    std::cerr << "\nError del servidor: " 
                              << (respuesta.contains("error") ? respuesta["error"] : "Respuesta inválida") 
                              << std::endl;
                }
            } catch (const json::exception& e) {
                std::cerr << "\nError al parsear la respuesta: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "\nError en la conexión: " << curl_easy_strerror(res) << std::endl;
        }
    }
    return false;
}

int main() {
    std::cout << "REGISTRO DE PERSONAS" << std::endl;
    std::cout << "--------------------" << std::endl;

    json datosPersona;

    std::cout << "Nombres: ";
    std::getline(std::cin, datosPersona["nombres"]);

    std::cout << "Apellidos: ";
    std::getline(std::cin, datosPersona["apellidos"]);

    std::cout << "Número de DPI: ";
    std::getline(std::cin, datosPersona["dpi"]);

    // Validar email
    do {
        std::cout << "Correo electrónico: ";
        std::getline(std::cin, datosPersona["email"]);
        if (!validarEmail(datosPersona["email"])) {
            std::cerr << "Formato de email inválido. Intente nuevamente.\n";
        }
    } while (!validarEmail(datosPersona["email"]));

    // Validar teléfono
    do {
        std::cout << "Número de teléfono: ";
        std::getline(std::cin, datosPersona["telefono"]);
        if (!validarTelefono(datosPersona["telefono"])) {
            std::cerr << "Formato de teléfono inválido. Use solo números.\n";
        }
    } while (!validarTelefono(datosPersona["telefono"]));

    std::cout << "Dirección: ";
    std::getline(std::cin, datosPersona["direccion"]);

    if (registrarPersona(datosPersona)) {
        std::cout << "Registro completado exitosamente!" << std::endl;
    } else {
        std::cerr << "Ocurrió un error al registrar." << std::endl;
    }

    return 0;
}