import sys
import re
from typing import List, Union, Tuple

def tokenize(code: str) -> List[str]:
    tokens = re.findall(r'\{|\}|\(|\)|=>|[a-zA-Z_]\w*|"[^"]*"|\S+', code)
    return [token.strip('"') for token in tokens]

def parse(tokens: List[str]) -> List[Union[str, List, Tuple]]:
    def parse_block():
        block = []
        while tokens and tokens[0] not in [')', '}']:
            if tokens[0] in ['(', '{']:
                tokens.pop(0)
                block.append(parse_block())
                if tokens and tokens[0] in [')', '}']:
                    tokens.pop(0)
            elif tokens[0] == 'map':
                tokens.pop(0)
                map_params = parse_block()
                map_body = parse_block()
                block.append(('map', map_params, map_body))
            else:
                block.append(tokens.pop(0))
        return block

    return parse_block()

def pretty_print(ast: Union[List, Tuple, str], indent: int = 0) -> None:
    if isinstance(ast, list):
        print("  " * indent + "[")
        for item in ast:
            pretty_print(item, indent + 1)
        print("  " * indent + "]")
    elif isinstance(ast, tuple) and ast[0] == 'map':
        print("  " * indent + "('map',")
        pretty_print(ast[1], indent + 1)
        pretty_print(ast[2], indent + 1)
        print("  " * indent + ")")
    else:
        print("  " * indent + repr(ast))

def main() -> None:
    tokens = tokenize(sys.argv[1])
    print(sys.argv[1])
    ast = parse(tokens)
    pretty_print(ast)

if __name__ == "__main__":
    main()