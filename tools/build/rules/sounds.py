import os

def build(runner, name, sound_file):
  #For now, just blindly copy the file, we'll add some processing
  runner.Copy(
    os.path.join(runner.ref_dir, sound_file),
    os.path.join(runner.dst_dir, 'sounds/', name + ".ogg"))
