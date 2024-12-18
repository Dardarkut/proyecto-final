#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <list>
#include <tuple>
#include <limits>

struct Product {
    std::string name;
    double price;
    int quantity;
};

using Request = std::tuple<std::string, std::string, int>;

std::map<std::string, Product> inventory;     
std::queue<Request> requests;                
std::queue<std::string> waitingClients;      
std::stack<std::tuple<std::string, Product>> changes; 
std::list<std::string> requestHistory;        

void clearConsole() {
    system("cls");
}

int getValidatedInt(const std::string& prompt, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < min || value > max) {
            clearConsole();
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Intente nuevamente.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

double getValidatedDouble(const std::string& prompt, double min = std::numeric_limits<double>::min()) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < min) {
            clearConsole();
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Intente nuevamente.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

std::string getValidatedString(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (!value.empty()) {
            return value;
        }
        clearConsole();
        std::cout << "Entrada invalida. Intente nuevamente.\n";
    }
}

void addProduct(const std::string& name, double price, int quantity) {
    clearConsole();
    inventory[name] = {name, price, quantity};
    if (!changes.empty()) changes.pop();
    changes.push({"add", {name, price, quantity}});
    std::cout << "Producto agregado: " << name << "\n";
}

void removeProduct(const std::string& name) {
    clearConsole();
    if (inventory.count(name)) {
        Product removedProduct = inventory[name];
        if (!changes.empty()) changes.pop();
        changes.push({"remove", removedProduct});
        inventory.erase(name);
        std::cout << "Producto eliminado: " << name << "\n";
    } else {
        std::cout << "Producto no encontrado.\n";
    }
}

void listProducts() {
    clearConsole();
    if (inventory.empty()) {
        std::cout << "Inventario vacio.\n";
        return;
    }
    for (const auto& item : inventory) {
        std::cout << item.second.name << " - Precio: " << item.second.price
                  << " - Cantidad: " << item.second.quantity << "\n";
    }
}

void undoChange() {
    clearConsole();
    if (changes.empty()) {
        std::cout << "No hay cambios para deshacer.\n";
        return;
    }
    auto lastChange = changes.top();
    changes.pop();
    std::string action = std::get<0>(lastChange);
    Product product = std::get<1>(lastChange);

    if (action == "add") {
        Product removedProduct = inventory[product.name];
        inventory.erase(product.name);
        changes.push({"remove", removedProduct});
        std::cout << "Se deshizo agregar: " << product.name << "\n";

    } else if (action == "remove") {
        inventory[product.name] = product;
        changes.push({"add", product});
        std::cout << "Se deshizo eliminar: " << product.name << "\n";
    }
}

void registerRequest(const std::string& client, const std::string& product, int quantity) {
    clearConsole();
    requests.push(std::make_tuple(client, product, quantity));
    std::cout << "Solicitud registrada para cliente: " << client << "\n";
}

void processRequest() {
    clearConsole();
    if (!requests.empty()) {
        Request currentRequest = requests.front();
        std::string client = std::get<0>(currentRequest);
        std::string product = std::get<1>(currentRequest);
        int quantity = std::get<2>(currentRequest);

        if (inventory.count(product) && inventory[product].quantity >= quantity) {
            inventory[product].quantity -= quantity;
            if (inventory[product].quantity == 0) {
                inventory.erase(product);
            }
            requests.pop();
            requestHistory.push_back(client + " - " + product + " - " + std::to_string(quantity));
            std::cout << "Solicitud procesada: " << client << " para " << product << " (" << quantity << ")\n";
        } else {
            std::cout << "No hay suficiente cantidad de " << product << " en el inventario para procesar la solicitud.\n";
        }
    } else {
        std::cout << "No hay solicitudes pendientes.\n";
    }
}

void listPendingRequests() {
    clearConsole();
    std::queue<Request> copy = requests;
    while (!copy.empty()) {
        Request req = copy.front();
        std::cout << std::get<0>(req) << " - Producto: " << std::get<1>(req)
                  << " - Cantidad: " << std::get<2>(req) << "\n";
        copy.pop();
    }
}

void listRequestHistory() {
    clearConsole();
    std::cout << "Historial de Solicitudes Procesadas:\n";
    for (const auto& record : requestHistory) {
        std::cout << record << "\n";
    }
}

void registerWaitingClient(const std::string& client) {
    clearConsole();
    waitingClients.push(client);
    std::cout << "Cliente registrado en espera: " << client << "\n";
}

void serveClient() {
    clearConsole();
    if (!waitingClients.empty()) {
        std::string currentClient = waitingClients.front();
        waitingClients.pop();
        std::cout << "Atendiendo a: " << currentClient << "\n";
    } else {
        std::cout << "No hay clientes en espera.\n";
    }
}

void viewWaitingClients() {
    clearConsole();
    std::queue<std::string> copy = waitingClients;
    while (!copy.empty()) {
        std::cout << copy.front() << "\n";
        copy.pop();
    }
}

int main() {
    int option;
    do {
        option = getValidatedInt(
            "\n=== MENU ===\n"
            "1. Agregar Producto\n"
            "2. Eliminar Producto\n"
            "3. Listar Productos\n"
            "4. Deshacer Cambio\n"
            "5. Registrar Solicitud\n"
            "6. Procesar Solicitud\n"
            "7. Listar Solicitudes\n"
            "8. Listar Historial de Solicitudes\n"
            "9. Registrar Cliente en Espera\n"
            "10. Atender Cliente\n"
            "11. Consultar Lista de Espera\n"
            "0. Salir\n"
            "Opcion: ",
            0, 11);

        clearConsole();
        switch (option) {
        case 1: {
            std::string name = getValidatedString("Nombre del producto: ");
            double price = getValidatedDouble("Precio del producto: ", 0.0);
            int quantity = getValidatedInt("Cantidad del producto: ", 0);
            addProduct(name, price, quantity);
            break;
        }
        case 2: {
            std::string name = getValidatedString("Nombre del producto a eliminar: ");
            removeProduct(name);
            break;
        }
        case 3:
            listProducts();
            break;
        case 4:
            undoChange();
            break;
        case 5: {
            std::string client = getValidatedString("Nombre del cliente: ");
            std::string product = getValidatedString("Nombre del producto: ");
            int quantity = getValidatedInt("Cantidad: ", 0);
            registerRequest(client, product, quantity);
            break;
        }
        case 6:
            processRequest();
            break;
        case 7:
            listPendingRequests();
            break;
        case 8:
            listRequestHistory();
            break;
        case 9: {
            std::string client = getValidatedString("Nombre del cliente: ");
            registerWaitingClient(client);
            break;
        }
        case 10:
            serveClient();
            break;
        case 11:
            viewWaitingClients();
            break;
        case 0:
            std::cout << "Saliendo...\n";
            break;
        default:
            std::cout << "Opcion invalida.\n";
        }
    } while (option != 0);

    return 0;
}
