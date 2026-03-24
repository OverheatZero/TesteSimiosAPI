FROM ubuntu:22.04
WORKDIR /app

RUN  apt-get update && apt install -y \
     build-essential \
     cmake \
     ninja-build \
     libasio-dev \
     libpqxx-dev \
     libpq-dev \
     git

COPY . .

RUN cmake -B build -G Ninja .
RUN cmake --build build

CMD ["./build/TesteSimiosAPI"]