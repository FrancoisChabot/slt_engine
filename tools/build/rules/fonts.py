import os

def build(runner, name, image_file):
  #For now, just blindly copy the file, we'll add some processing
  runner.Copy(
    os.path.join(runner.ref_dir, image_file),
    os.path.join(runner.dst_dir, 'fonts/', name + ".fnt"))
