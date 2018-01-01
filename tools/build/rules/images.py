import os

SLT_BIN_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SHADER_COMPILER = os.path.join(SLT_BIN_DIR, "shader_compiler")

def build(runner, name, image_file):
  #For now, just blindly copy the file, we'll add some processing
  runner.Copy(
    os.path.join(runner.ref_dir, image_file), 
    os.path.join(runner.dst_dir, 'images/', name + ".png"))