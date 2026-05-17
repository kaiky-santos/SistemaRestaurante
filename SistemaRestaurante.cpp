#include "SistemaRestaurante.hpp"

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>

Atendimento::Atendimento(unsigned int chefId, unsigned int mesaId) {
    this->chefId = chefId;
    this->mesaId = mesaId;
    
    if (pipe(fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pid = fork(); pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        
        iniciar();
        _exit(0);
    } else {
        
        close(fd[0]);
        usleep(1000); // vi que sem isso, o primeiro pedido nao era gravado, o filho ainda nao entrou no loop de leitura quando o pai ja escreveu no pipe
    }
}

Atendimento::~Atendimento() {
    
    if (pid > 0) {
        close(fd[1]);
        kill(pid, SIGKILL);

    } else {
        close(fd[0]);
    }
}

void Atendimento::prepararPedido(const std::string &pedido) const {
    
    write(fd[1], pedido.c_str(), pedido.size() + 1);
}

void Atendimento::iniciar() {

    pid = getpid();

    close(fd[1]);

    std::ofstream arquivo("ChefeCozinha_" + std::to_string(chefId) + ".txt", std::ios::app);
    arquivo << "Mesa " << mesaId << ":\n";

    char buffer[256]; // buffer para ler as requisições
    while (true) {
        // chama a função read (de forma bloqueante) lendo o descritor do pipe e adicionando o conteúdo em buffer
        const ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        buffer[n] = '\0'; // adiciona o caractere nulo ao final do buffer
        std::string msg(buffer); // converte o buffer (array de char) em uma string
        arquivo << "- " << msg << "\n";
        arquivo.flush();
    }
    arquivo.close();
    close(fd[0]);
}

Chef::Chef(const unsigned int id) : id(id), atendimento(nullptr) {
}

void Chef::iniciarAtendimento(const unsigned int mesa) {
    atendimento = new Atendimento(id, mesa);
}

void Chef::prepararPedido(const std::string &pedido) {
    
    atendimento->prepararPedido(pedido);
}

void Chef::encerrarAtendimento() {
    

    delete atendimento;
    atendimento = nullptr;
}

SistemaRestaurante::SistemaRestaurante(int totalChefs, int totalMesas){

    this->totalMesas = totalMesas;
    chefs.reserve(totalChefs); 
    for (int i = 1; i <= totalChefs; i++) {
        chefs.push_back(Chef(i));
        disponiveis.push_back(&chefs.back());
    }
}

void SistemaRestaurante::atenderPedido(int mesa, std::string &pedido) {
         
    if (mesaParaChef.count(mesa)){
        mesaParaChef[mesa]->prepararPedido(pedido);

    } else if (!disponiveis.empty()) {    // se tiver chefe disponivel
        Chef* chef = disponiveis.front(); // pega o primeiro chef
        mesaParaChef[mesa] = chef;        // vincula ao mapa
        disponiveis.pop_front();          // remove da lista de disponíveis
        chef->iniciarAtendimento(mesa);   // inicia o processo filho
        chef->prepararPedido(pedido);     // manda o pedido

    
    } else {
        if (!filaDePedidos.count(mesa)) {
            filaOrdemChegada.push(mesa); // só adiciona na ordem se for a primeira vez
        }
        filaDePedidos[mesa].push(pedido);
    }   
}

void SistemaRestaurante::encerrarMesa(int mesa){

    if (!mesaParaChef.count(mesa)){
        if (!filaDePedidos.count(mesa)) {
            filaOrdemChegada.push(mesa); // ← adiciona na ordem também
        }
        filaDePedidos[mesa].push("fim");
        return;
}

    Chef* chef = mesaParaChef[mesa]; //pego o chefe do mapa
    chef->encerrarAtendimento();     // encero o atendimento do chefe
    mesaParaChef.erase(mesa);        //removo a mesa do mapa
    
    if (filaDePedidos.empty()){

        disponiveis.push_back(chef);
    } else {
         consumirPedidoFila(chef);
    }
    
}

void SistemaRestaurante::consumirPedidoFila(Chef* chef){

    int mesa = filaOrdemChegada.front(); // pega a mesa na ordem de chegada
    filaOrdemChegada.pop();              // remove da fila de ordem
    auto it = filaDePedidos.find(mesa);

    mesaParaChef[mesa] = chef;
    chef->iniciarAtendimento(mesa);

    while (!it->second.empty()){ //se nao estiver vazia, ou seja, enquanto tiver pedidos
        
        std::string pedido = it->second.front(); 
        it->second.pop();

        if (pedido == "fim"){

            usleep(100000);
            chef->encerrarAtendimento();
            mesaParaChef.erase(mesa);
            filaDePedidos.erase(it);
            if (filaDePedidos.empty()){
                disponiveis.push_back(chef);

            } else {
                consumirPedidoFila(chef);
            }
            return;
        } else {
            chef->prepararPedido(pedido);
            usleep(20000);
        }
    }
    filaDePedidos.erase(it);
}

    

