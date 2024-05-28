FROM arm64v8/debian:latest

RUN apt-get update                 \
    && apt-get upgrade -y          \
    && apt-get install -y sudo     \
    && rm -rf "/var/lib/apt/lists/*"

COPY "scripts/install-tools.sh" "/root"
COPY "scripts/init.sh" "/root"
COPY "dotfiles/.bashrc" "/root"
COPY "dotfiles/.inputrc" "/root"

RUN sh "/root/install-tools.sh"

ENTRYPOINT [ "sh", "/root/init.sh" ]
