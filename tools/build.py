#!/usr/bin/env python3
"""Build through the pinned GBA Studio CLI and project-scoped native bootstrap."""
from pathlib import Path
import argparse,json,os,platform,shutil,subprocess,sys,tarfile
r=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('--devkitpro',type=Path,help='Existing devkitPro directory containing devkitARM and tools')
ap.add_argument('--studio-resources',type=Path,help='Installed GBA Studio 0.5.0 resources directory, for its compiler archive')
ap.add_argument('--clean',action='store_true',help='Remove generated .gbastudio output before building')
args=ap.parse_args()
devkit=args.devkitpro
if not devkit:
 resources=args.studio_resources or (Path(os.environ['GBA_STUDIO_RESOURCES']) if os.environ.get('GBA_STUDIO_RESOURCES') else None)
 if not resources:ap.error('Use --devkitpro PATH or --studio-resources PATH. See README.md. The shipped ROM needs no compiler.')
 system={'Linux':'linux','Windows':'win32','Darwin':'darwin'}.get(platform.system())
 devkit=r/'.toolchain'/system
 if not (devkit/'devkitARM').exists():
  archive=resources/'public/vendors/devkitPro'/f'{system}.tar.gz'
  if not archive.is_file():ap.error(f'Compiler archive not found: {archive}')
  devkit.mkdir(parents=True,exist_ok=True)
  with tarfile.open(archive) as f:
   for member in f.getmembers():
    dest=(devkit/member.name).resolve()
    if not dest.is_relative_to(devkit.resolve()):raise RuntimeError('Invalid archive member')
   f.extractall(devkit,filter='tar' if sys.version_info>=(3,12) else None)
if os.name != 'nt':
 for directory in ['devkitARM/bin','devkitARM/libexec','devkitARM/arm-none-eabi/bin','tools/bin']:
  for executable in (devkit/directory).rglob('*'):
   if executable.is_file():
    with executable.open('rb') as binary: magic=binary.read(4)
    if magic==b'\x7fELF':executable.chmod(executable.stat().st_mode|0o111)
devkit=devkit.resolve()
compiler=devkit/'devkitARM/bin'/('arm-none-eabi-g++.exe' if os.name=='nt' else 'arm-none-eabi-g++')
if not compiler.is_file():ap.error(f'Compiler not found: {compiler}')
if not shutil.which('node') or not shutil.which('make'):ap.error('Install Node.js 22+ and GNU Make and ensure both are on PATH.')
project=r/'PokemonBattleArena.gbasproj';data=json.loads(project.read_text());data['settings']['pythonPath']=sys.executable.replace('\\','/');data['settings']['devkitProPath']=str(devkit).replace('\\','/');project.write_text(json.dumps(data,indent=2)+'\n')
subprocess.run([sys.executable,str(r/'tools/generate_config.py')],check=True,cwd=r)
if args.clean and (r/'.gbastudio').exists():shutil.rmtree(r/'.gbastudio')
env=dict(os.environ,GBA_STUDIO_RESOURCES=str(r/'engine/studio_resources'))
command=['node',str(r/'tools/studio_cli.mjs'),'build']
result=subprocess.run(command,cwd=r,env=env)
if result.returncode:
 # A failed compiler invocation can leave an empty generated object whose
 # timestamp then prevents make from rebuilding it. Retry only this case.
 empty=[p for p in (r/'.gbastudio/build').glob('*.o') if p.stat().st_size==0]
 if not empty:raise subprocess.CalledProcessError(result.returncode,command)
 for p in empty:p.unlink()
 print('Retrying invalid empty generated object(s): '+', '.join(p.name for p in empty),flush=True)
 subprocess.run(command,check=True,cwd=r,env=env)
rom=r/'out/PokemonBattleArena.gba'
if not rom.is_file() or rom.stat().st_size<192:raise RuntimeError('Build produced no valid ROM')
print(f'Playable ROM: {rom}')
