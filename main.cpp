#include <iostream>
#include <crow.h>
#include <iostream>
#define SEQUENCIA_MIN 4

bool isSimian(std::string dna[]) {

    const int tamanho = dna[0].size();
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {

            char posicaoAtual = dna[i][j];

            if (j <= tamanho - SEQUENCIA_MIN &&
                posicaoAtual == dna[i][j + 1] &&
                posicaoAtual == dna[i][j + 2] &&
                posicaoAtual == dna[i][j + 3])
                {

                return true;
            }
            if (i <= tamanho - SEQUENCIA_MIN &&
                posicaoAtual == dna[i + 1][j] &&
                posicaoAtual == dna[i + 2][j] &&
                posicaoAtual == dna[i + 3][j])
                {
                return true;
            }
            if (i <= tamanho - SEQUENCIA_MIN && j <= tamanho - SEQUENCIA_MIN &&
                posicaoAtual == dna[i + 1][j + 1] &&
                posicaoAtual == dna[i + 2][j + 2] &&
                posicaoAtual == dna[i + 3][j + 3])
                {
                return true;
            }
            if (i <= tamanho - SEQUENCIA_MIN && j >= SEQUENCIA_MIN - 1 &&
                posicaoAtual == dna[i + 1][j - 1] &&
                posicaoAtual == dna[i + 2][j - 2] &&
                posicaoAtual == dna[i + 3][j - 3])
                {
                return true;
            }
        }
    }
    return false;
}

int main() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/simian").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
        auto body = crow::json::load(req.body);

        if (!body)
            return crow::response(400, "JSON inválido");

        std::vector<std::string> valores;
        for (const auto& linha : body["dna"]) {
            valores.push_back(std::string(linha.s()));
        }

        int size = valores.size();
        std::string linhas[size];
        for (int i = 0; i < size; i++) {
            linhas[i] = valores[i];
        }

       if (isSimian(linhas)) {
           return crow::response(200);
       }else {
           return crow::response(403);
       }
    });

    app.port(18080).run();

    return 0;
}