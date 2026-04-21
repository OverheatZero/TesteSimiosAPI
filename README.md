#Importante:
Os endpoints não funcionam mais, já que o período de avaliação gratuita no Railway acabou :/
Mas ainda pode ser rodado localmente com as instruções abaixo!

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

    sequenciadna: TEXT (Chave Primária para evitar duplicatas).
    resultado: TEXT (Valores: "Simio" ou "Humano").

A tabela foi gerada da seguinte forma:


```SQL
CREATE TABLE Dna (
sequenciadna TEXT PRIMARY KEY,
resultado TEXT NOT NULL
);
``` 

O banco de dados é provisionado via Railway, com variáveis de ambiente injetadas no container. 
  
## Como Executar localmente via Docker

O projeto já possui um `Dockerfile` que configura todas as dependências necessárias, como `build-essential`, `cmake`, `libasio-dev` e as bibliotecas do Postgres.

1.  **Construir a imagem:**
2.  Na raíz do projeto:
    ```bash
    docker build -t teste-simios-api .
    ```

3.  **Executar o container:**
    É necessário passar as variáveis de ambiente para a conexão com um banco PostgreSQL:
    ```bash
    docker run -p 18080:18080 \
      -e PGHOST=seu_host \
      -e PGDATABASE=seu_db \
      -e PGUSER=seu_usuario \
      -e PGPASSWORD=sua_senha \
      -e PGPORT=5432 \
      teste-simios-api
    ```
Após isso a API fica acessível localmente:

- http://localhost:18080/simian
- http://localhost:18080/stats

## Executando localmente sem Docker (Opcional)

Caso deseje rodar a aplicação localmente, é necessário configurar variáveis de ambiente para conexão com um banco PostgreSQL.

### Pré-requisitos

- CMake (>= 3.22)
- Compilador C++ com suporte a C++17
- libpqxx

### Build e execução

```bash
mkdir build
cd build
cmake ..
make
./TesteSimiosAPI
```

### Variáveis de Ambiente
```bash
export PGHOST=seu_host
export PGDATABASE=seu_db
export PGUSER=seu_usuario
export PGPASSWORD=sua_senha
export PGPORT=5432
```
Após isso a API fica acessível localmente:

- http://localhost:18080/simian
- http://localhost:18080/stats
  

## Endpoints da API

A API utiliza a porta **18080**.

### 1. Verificar DNA (`POST /simian`)
Analisa se um DNA é símio ou humano.
* **Corpo da Requisição (JSON):**
    ```json
    {
      "dna": ["ATGCGA", "CAGTGC", "TTATGT", "AGAAGG", "CCCCTA", "TCACTG"]
    }
    ```
O campo "dna" é obrigatório no corpo da requisição, caso contrário, a API não conseguirá encontrar a
matriz de DNA e retornará o erro 400 (Bad Request)!

* **Respostas:**
    * `200 OK`: Se for detectado um símio (sequência encontrada).
    * `403 Forbidden`: Se for detectado um humano.
    * `400 Bad Request`: Formato de DNA inválido ou caracteres fora do padrão (A, T, C, G).

Teste utilizando terminal Linux:
 ```bash
curl -i -X POST https://testesimiosapi-production.up.railway.app/simian \
-H "Content-Type: application/json" \
-d '{"dna":["ATGCGA","CAGTGC","TTATGT","AGAAGG","CCCCTA","TCACTG"]}'
```
Teste utilizando PowerShell do Windows:
```bash
$body = @{
  dna = @("ATGCGA","CAGTGC","TTATGT","AGAAGG","CCCCTA","TCACTG")
} | ConvertTo-Json -Compress
```
```
Invoke-RestMethod `
  -Uri "https://testesimiosapi-production.up.railway.app/simian" `
  -Method POST `
  -Body $body `
  -ContentType "application/json; charset=utf-8"
```
  

### 2. Estatísticas (`GET /stats`)
Retorna um JSON com as quantidades de DNA verificado no banco de dados e a proporção entre eles.
```json
{
"ratio": 0.4,
"count_mutant_dna": 40,
"count_human_dna": 100
}
```
Teste utilizando terminal Linux:
```bash
curl -i https://testesimiosapi-production.up.railway.app/stats
```

Teste utilizando PowerShell do Windows:
```bash
Invoke-RestMethod -Uri "https://testesimiosapi-production.up.railway.app/stats"
```
Observação: O campo ```ratio``` será 0 quando não houver registros (de humanos ou símios) suficientes para cálculo (evitando divisão por zero).


## 🔍 Regras de Validação

* **Matriz Quadrada:** O DNA deve ser uma matriz $NxN$ (ex: 4x4, 6x6, etc.).
* **Caracteres Permitidos:** Apenas 'A', 'T', 'G' e 'C'.
* **Persistência:** Os resultados são salvos na tabela `"Dna"`, e duplicatas são evitadas através de chave primária e uso da cláusula SQL ```ON CONFLICT DO NOTHING.```
