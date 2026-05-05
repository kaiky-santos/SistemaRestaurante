#include <iostream>
#include <thread>
#include <vector>
#include <csignal>   // ou <signal.h>


#include "SistemaRestaurante.hpp"

int main() {
    signal(SIGCHLD, SIG_IGN); // evita filhos zumbis (pesquisem sobre...)

}
