import os

SLT_BIN_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SHADER_COMPILER = os.path.join(SLT_BIN_DIR, "shader_compiler")

def build(runner, name, root_file):
  runner.Build(SHADER_COMPILER, {
    "input": os.path.join(runner.ref_dir, root_file), 
    "output": os.path.join(runner.dst_dir, 'shaders/', name + ".cap")
    })