"""Build the one-core Lua/keyboard harness against its pinned upstream mGBA build."""
from pathlib import Path
import argparse
import shlex
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--mgba-source', type=Path, required=True)
parser.add_argument('--mgba-build', type=Path, required=True)
parser.add_argument('--output', type=Path, default=Path('.tooling/mgba_versus_driver'))
args = parser.parse_args()
source, build = args.mgba_source.resolve(), args.mgba_build.resolve()
revision = subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip()
if revision != 'a67176cd3e6bf60a5016d7adc94357142dd12617':
    parser.error('Use the documented pinned mGBA development revision')
flags = (build / 'CMakeFiles/mgba.dir/flags.make').read_text()
definitions = next(line.split('=', 1)[1] for line in flags.splitlines()
                   if line.startswith('C_DEFINES ='))
cache = (build / 'CMakeCache.txt').read_text()
lua = next(line.split('=', 1)[1] for line in cache.splitlines()
           if line.startswith('LUA_LIBRARY:'))
root = Path(__file__).resolve().parents[1]
args.output.parent.mkdir(parents=True, exist_ok=True)
subprocess.run(['cc', *shlex.split(definitions), str(root / 'tests/mgba_versus_driver.c'),
                '-I' + str(source / 'include'), '-I' + str(build / 'include'),
                str(build / 'libmgba.a'), lua, '-lm', '-ldl', '-lpthread',
                '-o', str(args.output)], check=True)
print('Built', args.output, 'against upstream mGBA', revision)
