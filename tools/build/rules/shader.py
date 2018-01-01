import platform
import os

SLT_BIN_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SHADER_COMPILER = os.path.join(SLT_BIN_DIR, 'shader_compiler')

if platform.system() == 'Windows':
    SHADER_COMPILER += '.exe'

def build(runner, name, root_file):
  shader_dir = os.path.join(runner.dst_dir, 'shaders')
  os.makedirs(shader_dir, exist_ok=True)
  runner.Build(SHADER_COMPILER, {
    "input": os.path.join(runner.ref_dir, root_file),
    "output": os.path.join(shader_dir, name + ".cap")
    })
