import bip.build
from pathlib import Path
from os import unlink

def clean(bld: bip.build.Info) -> bool:
  out = bld.root_dir.joinpath("data")
  if not out.exists():
    return True

  for e in out.iterdir():
    if e.is_dir() or not e.name.endswith(".bndl"):
      continue

    bld.log.verbose(f"Removing {e}")
    unlink(e)

  return True

g_should_build: list[tuple[Path, Path]] = []

def should_build(bld: bip.build.Info) -> bool:
  src = bld.src_dir.joinpath("data")
  if not src.exists():
    return False

  for bndldir in src.iterdir():
    if not bndldir.is_dir():
      continue

    bndl = bld.root_dir.joinpath("data", bndldir.name).with_suffix(".bndl")
    if not bndl.exists():
      g_should_build.append((bndldir, bndl))
      continue

    bndlmt = bndl.stat().st_mtime
    for bndlfile in bndldir.iterdir():
      if bndlfile.stat().st_mtime >= bndlmt:
        g_should_build.append((bndldir, bndl))
        continue

  return len(g_should_build) > 0

def build(bld: bip.build.Info) -> bool:
  for bndldir, bndl in g_should_build:
    if not bip.common.cmd(bld.log, "nwge-bndl", ["create", str(bndldir), str(bndl)]):
      return False

  return True
