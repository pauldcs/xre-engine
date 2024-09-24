from core.config import Config
from core.server import Server


def __start__(conf: Config) -> bool:
    server = Server(conf)
    server.start_server()