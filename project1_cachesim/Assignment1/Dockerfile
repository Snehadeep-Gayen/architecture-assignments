FROM --platform=linux/amd64 ubuntu:22.04
RUN echo "Acquire::http::Pipeline-Depth 0;" > /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::http::No-Cache true;" >> /etc/apt/apt.conf.d/99custom && \
    echo "Acquire::BrokenProxy    true;" >> /etc/apt/apt.conf.d/99custom
RUN rm -rf /var/lib/apt/lists/*
RUN apt-get clean
RUN apt-get update -o Acquire::CompressionTypes::Order::=gz
RUN apt-get upgrade -y
RUN apt-get update -y
RUN apt-get clean
RUN apt-get install -y vim
RUN apt-get install -y g++
RUN apt-get install -y curl
RUN apt-get install -y build-essential
WORKDIR /app
COPY . .