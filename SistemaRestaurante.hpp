#pragma once

#include <string>
#include <map>
#include <list>
#include <queue>
#include <vector>
#include <utility>

// Esta classe representa um atendimento. O atendimento terá um subprocesso vinculado a ele
class Atendimento final {
public:
    // Construtor da classe. Para se criar um atendimento, é necessário informar o ID do chef e da mesa.
    // Ao se criar um objeto Atendimento, um subprocesso pronto com pipes para receber comunicação entre processos é gerado
    Atendimento(unsigned int chefId, unsigned int mesaId);

    // Destrutor da classe. Ao invocá-lo, o subprocesso vinculado a este atendimento deve ser encerrado.
    ~Atendimento();

    void prepararPedido(const std::string &pedido) const;

private:
    // ID do processo (pid)
    pid_t pid{-1};
    // Descritores de arquivo que guardam o "número dos canais" onde serão realizadas a comunicação entre processos.
    // São dois inteiros, inicializados previamente com o valor -1
    int fd[2]{-1, -1};
    unsigned int chefId;
    unsigned int mesaId;
    

    // Método chamado na inicialização do atendimento
    void iniciar();
};

class Chef {
public:
    explicit Chef(unsigned int id);

    void iniciarAtendimento(unsigned int mesa);

    void prepararPedido(const std::string &pedido);

    void encerrarAtendimento();

private:
    const unsigned int id;
    Atendimento *atendimento;
};

class SistemaRestaurante {
private:
    std::vector<Chef> chefs;
    std::list<Chef*> disponiveis;
    std::unordered_map<int, Chef*> mesaParaChef;
    std::map<int, std::queue<std::string>> filaDePedidos;
    std::queue<int> filaOrdemChegada;
   
    int totalMesas;

public:
    SistemaRestaurante(int totalChefs, int totalMesas);

    void atenderPedido(int mesa, std::string &descricaoPedido);

    void encerrarMesa (int mesa);

    void consumirPedidoFila (Chef* chef);
    

};
