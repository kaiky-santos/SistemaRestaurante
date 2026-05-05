#include "SistemaRestaurante.hpp"

#include <iostream>
#include <csignal>
#include <unistd.h>

Atendimento::Atendimento(unsigned int chefId, unsigned int mesaId) {
    
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
    }
}

Atendimento::~Atendimento() {
    
    if (pid > 0) {
        close(fd[1]);
        // Mata o subprocesso filho usando a função kill para isso.
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
    std::cout << "Atendimento iniciado: " << getpid() << std::endl;

    close(fd[1]);
    char buffer[256]; // buffer para ler as requisições
    while (true) {
        // chama a função read (de forma bloqueante) lendo o descritor do pipe e adicionando o conteúdo em buffer
        const ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        buffer[n] = '\0'; // adiciona o caractere nulo ao final do buffer
        std::string msg(buffer); // converte o buffer (array de char) em uma string
        std::cout << pid << " preparando o pedido: " << msg << std::endl;
    }
    close(fd[0]);
}

Chef::Chef(const unsigned int id) : id(id), atendimento(nullptr) {
}

void Chef::iniciarAtendimento(const unsigned int mesa) {
    atendimento = new Atendimento(id, mesa);
}

void Chef::prepararPedido(const std::string &pedido) {
    if (atendimento == nullptr) {
        iniciarAtendimento(10); // vinculei a uma mesa qualquer somente como exemplo
    }
    atendimento->prepararPedido(pedido);
}

void Chef::encerrarAtendimento() {
    

    delete atendimento;
    atendimento = nullptr;
}
