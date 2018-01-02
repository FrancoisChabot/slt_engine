import platform
import os

SLT_BIN_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
REGISTRY_COMPILER = os.path.join(SLT_BIN_DIR, 'registry_compiler')

if platform.system() == 'Windows':
    REGISTRY_COMPILER += '.exe'

def build(runner, root_file):
  runner.Build(REGISTRY_COMPILER, {
    "input": os.path.join(runner.ref_dir, root_file),
    "output": os.path.join(runner.dst_dir, "reg.cap")
    })
