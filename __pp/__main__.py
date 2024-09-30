import sys
from optparse import OptionParser

from __start__ import __start__
from core.config import Config
from support.__constants__ import PROGRAM, __date__, __version__


def main():
    opts = OptionParser(
        usage=(f'./{PROGRAM} [OPTIONS]\n'
            f'v{__version__} ({__date__})')
    )

    opts.add_option('-v', '--verbose', type=bool, default=False,
                    help=f'Enable verbose mode (more logging)')

    (o, _) = opts.parse_args()
    
    conf = Config(
        verbose==o.verbose,
    )

    return(__start__(conf))

if __name__ == "__main__":
    sys.exit(main())