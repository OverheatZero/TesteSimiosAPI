# TesteSimiosAPI 

API REST em C++ para detecção de DNA símio em matrizes NxN, com persistência em PostgreSQL e deploy em produção. O projeto utiliza o micro-framework **Crow** para a camada HTTP e **PostgreSQL** para persistência de dados.

## Hospedagem (Railway)

A API está disponível publicamente nos seguintes endereços:

    Estatísticas: https://testesimiosapi-production.up.railway.app/stats

    Verificação: https://testesimiosapi-production.up.railway.app/simian

Ambos os endereços podem ser testados utilizando serviços como Postman e Insomnia.
    
## Tecnologias e Dependências

* **Linguagem:** C++17
* **Framework Web:** [CrowCpp](https://github.com/CrowCpp/Crow) (Gerenciado via `FetchContent` no CMake).
* **Banco de Dados:** PostgreSQL (utilizando `libpqxx` e `libpq`).
* **Build System:** CMake (mínimo 3.22) e Ninja.
* **Ambiente:** Docker (Baseado em Ubuntu 22.04).

## Estrutura do Projeto

O projeto utiliza os seguintes componentes:

- ```main.cpp```: Contém a lógica de detecção (busca horizontal, vertical e diagonal), as rotas da API (/simian e /stats) e a integração com o banco de dados via libpqxx.

- ```CMakeLists.txt```: Gerencia as dependências, utilizando FetchContent para o framework Crow e PkgConfig para as bibliotecas do Postgres.

- ```Dockerfile```: Configura um ambiente Ubuntu 22.04, instala dependências como build-essential, cmake, ninja-build e as bibliotecas libpqxx-dev.

## Estrutura do Banco de Dados

A API espera uma tabela chamada Dna com a seguinte estrutura lógica:

    sequenciadna: String/Texto (Chave Primária para evitar duplicatas).

    resultado: String/Texto (Valores: "Simio" ou "Humano").

O banco de dados é provisionado via Railway, com variáveis de ambiente injetadas no container.
  
## Como Executar via Docker

O projeto já possui um `Dockerfile` que configura todas as dependências necessárias, como `build-essential`, `cmake`, `libasio-dev` e as bibliotecas do Postgres.

1.  **Construir a imagem:**
    ```bash
    docker build -t teste-simios-api .
    ```

2.  **Executar o container:**
    É necessário passar as variáveis de ambiente para a conexão com o banco de dados:
    ```bash
    docker run -p 18080:18080 \
      -e PGHOST=seu_host \
      -e PGDATABASE=seu_db \
      -e PGUSER=seu_usuario \
      -e PGPASSWORD=sua_senha \
      -e PGPORT=5432 \
      teste-simios-api
    ```

## Endpoints da API

A API escuta na porta **18080**.

### 1. Verificar DNA (`POST /simian`)
Analisa se um DNA é símio ou humano.
* **Corpo da Requisição (JSON):**
    ```json
    {
      "dna": ["ATGCGA", "CAGTGC", "TTATGT", "AGAAGG", "CCCCTA", "TCACTG"]
    }
    ```
O nome do vetor dentro do JSON sempre deve ser "dna", caso contrário, a API não conseguirá encontrar a
matriz de DNA e retornará o erro 400 (Bad Request)!

* **Respostas:**
    * `200 OK`: Se for detectado um símio (sequência encontrada).
    * `403 Forbidden`: Se for detectado um humano.
    * `400 Bad Request`: Formato de DNA inválido ou caracteres fora do padrão (A, T, C, G).

Teste utilizando terminal Linux:
 ```bash
curl -i -X POST https://testesimiosapi-production.up.railway.app/simian
-H "Content-Type: application/json"
-d '{"dna":["ATGCGA","CAGTGC","TTATGT","AGAAGG","CCCCTA","TCACTG"]}'
```
  

### 2. Estatísticas (`GET /stats`)
Retorna um JSON com as quantidades de DNA verificado no banco de dados e a proporção entre eles.
```json
{
"count_mutant_dna": 40,
 "count_human_dna": 100,
 "ratio": 0.4
}
```
Teste utilizando terminal Linux:
```bash
curl -i https://testesimiosapi-production.up.railway.app/stats
```

## 🔍 Regras de Validação

* **Matriz Quadrada:** O DNA deve ser uma matriz $NxN$.
* **Caracteres Permitidos:** Apenas 'A', 'T', 'G' e 'C'.
* **Persistência:** Os resultados são salvos na tabela `"Dna"`, e duplicatas são evitadas através de chave primária e uso de ```ON CONFLICT DO NOTHING.```
