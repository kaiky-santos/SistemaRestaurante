#include <iostream>
#include <thread>
#include <vector>
#include <csignal>   // ou <signal.h>


#include "SistemaRestaurante.hpp"

int main() {
    signal(SIGCHLD, SIG_IGN);

    int totalChefs, totalMesas;
    std:: cin >> totalChefs >> totalMesas;
    std::cin.ignore();
    SistemaRestaurante sistema (totalChefs, totalMesas);

   
    std::string linha;
    while (std::getline (std::cin, linha)){
        
        if (linha == "FIM") break;


        int mesa = std::stoi(linha);
        std::string pedido = linha.substr(linha.find(' ') + 1);  
        
        if (pedido == "fim"){
            sistema.encerrarMesa(mesa);
        } else {
            sistema.atenderPedido(mesa, pedido);

        }
    }
    return 0;
}
