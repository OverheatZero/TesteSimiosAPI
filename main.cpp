#include <iostream>
#include <crow.h>
#include <pqxx/pqxx>
#include <cstdlib>

#include <iostream>
#include <bits/stl_pair.h>
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

bool isValid(const std::vector<std::string>& dna) {
    if (dna.empty()) {
        return false;
    }
    int tamanhoLinha = dna[0].size();
    int quantidadeDeLinhas = dna.size();

    if (tamanhoLinha != quantidadeDeLinhas) {
        return false;
    }

    for (const std::string& linha : dna) {
        if (linha.size()!= tamanhoLinha) {
            return false;
        }
        for (char caractere : linha) {
            if (caractere != 'A' && caractere != 'T' && caractere != 'G' && caractere != 'C') {
                return false;
            }
        }
    }
    return true;
}

int main() {

    std::string dbHost = std::getenv("PGHOST");
    std::string dbName = std::getenv("PGDATABASE");
    std::string dbUser = std::getenv("PGUSER");
    std::string dbPwd = std::getenv("PGPASSWORD");
    std::string dbPort = std::getenv("PGPORT");

    std::string conexao = "dbname=" + dbName + " user=" + dbUser + " password=" + dbPwd + " host=" + dbHost + " port=" + dbPort;

    try {
        pqxx::connection c(conexao);
    }catch (std::exception& e) {
        std::cout << "Erro na conexão ao banco!";
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/simian").methods(crow::HTTPMethod::POST)([conexao](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "JSON inválido");

        if (!body.has("dna") || body["dna"].t() != crow::json::type::List) {
            return crow::response(400, "Campo dna não encontrado!");
        }

        std::vector<std::string> valores;
        for (const auto& linha : body["dna"]) {
            valores.push_back(linha.s());
        }
        if (!isValid(valores)) {
            return crow::response(400, "Formato de DNA inválido!");
        }
        int size = valores.size();
        std::string linhas[size];
        for (int i = 0; i < size; i++) {
            linhas[i] = valores[i];
        }
        bool simian = isSimian(linhas);

        std::string sequenciaDna;
        for (int i = 0; i < size; i++) {
            sequenciaDna += linhas[i];
            if (i < size - 1) {
                sequenciaDna += ";";
            }
        }
        std::string resultado;


        if (simian) {
            resultado = "Simio";
        }else {
            resultado = "Humano";
        }

        try {
            pqxx::connection c(conexao);
            pqxx::work transaction(c);
            transaction.exec_params("INSERT INTO \"Dna\" VALUES ($1, $2) ON CONFLICT DO NOTHING",
                             sequenciaDna,
                             resultado);
            transaction.commit();
        }catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
    }

       if (simian) {
           return crow::response(200, "Simio\n");
       }else {
           return crow::response(403, "Humano\n");
       }
    });
    CROW_ROUTE(app, "/stats").methods(crow::HTTPMethod::GET)([conexao]() {
        int simios;
        int humanos;
        double ratio = 0.0;
        try {
            pqxx::connection c(conexao);
            pqxx::work transaction(c);
            pqxx::result s = transaction.exec_params("SELECT COUNT(*) AS simios FROM \"Dna\" WHERE resultado = $1",
                                    "Simio");
            pqxx::result h = transaction.exec_params("SELECT COUNT(*) AS humanos FROM \"Dna\" WHERE resultado = $1",
                                    "Humano");
            simios = s[0][0].as<int>();
            humanos = h[0][0].as<int>();
            if (humanos != 0) {
                ratio = (double) simios / humanos;
            }
            transaction.commit();

        }catch (std::exception& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
        }

        crow::json::wvalue retorno;

        retorno["count_mutant_dna"] = simios;
        retorno["count_human_dna"] = humanos;
        if (ratio > 0) {
            retorno["ratio"] = ratio;
        }else {
            retorno["ratio"] = "Ratio = 0.0 quando não há humanos.";
        }

        return retorno;
    });

    app.port(18080).run();

    return 0;
}