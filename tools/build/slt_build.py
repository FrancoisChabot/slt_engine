import argparse
import os
import sys
import hashlib
import itertools
import json
import time
import subprocess as proc
import rules.shader as shader
import rules.images as images

def _ArgsDictToList(args):
  return ['--' + name + '=' + val for name, val in sorted(args.items())]

def _getHash(cmds):
  m = hashlib.sha256()
  for cmd in cmds:
    for c in cmd:
      m.update(c.encode('utf-8'))

  return m.hexdigest()

rules = {
  "shaders" : shader.build,
  "images" : images.build
}

class Runner:
  def __init__(self, cache_dir, ref_dir, dst_dir):
    self._cache_dir = cache_dir
    self.ref_dir = ref_dir
    self.dst_dir = dst_dir
    os.makedirs(self._cache_dir, exist_ok=True)

  def Copy(self, src, dst):
    cmd_1 = ['cmake', '-E', 'copy_if_different', src, dst]
    cmd_2 = ['cmake', '-E', 'touch', dst]
    self.Exec([cmd_1, cmd_2], [src], [dst])

  def Exec(self, cmds, inputs, outputs):
    should_run = False
    cache_file_name = _getHash(cmds) + '.json'
    cache_file = os.path.join(self._cache_dir, cache_file_name)

    cache = {
      "inputs" : [],
      "outputs" : [],
      "timestamp" : 0,
      "success" : False,
      "stdout" : [],
      "stderr" : [],
      "cmd" : []
    }
    should_run = False

    if os.path.isfile(cache_file):
      with open(cache_file) as cache_data:
        cache = json.load(cache_data)

      if cache['success'] == False:
        print('      Last run failed')
        should_run = True

      for i in cache['inputs']:
        if os.path.isfile(i):
          ts = os.path.getmtime(i)
          if ts > cache['timestamp']:
            print('      ' + i + ' has been modified')
            should_run = True
        else:
          print('      ' + i + ' could not be found')
          should_run = True

      for i in cache['outputs']:
        if os.path.isfile(i):
          ts = os.path.getmtime(i)
          if ts < cache['timestamp']:
            print('      ' + i + ' is stale')
            print(ts)
            print(cache['timestamp'])
            should_run = True
        else:
          print('      ' + i + ' could not be found')
          should_run = True

    else:
      print("      no cache found")
      should_run = True

    if should_run:
      cache['timestamp'] = int(time.time())
      cache['cmd'] = [ ' '.join(cmd) for cmd in cmds ]
      cache['inputs'] = [__file__]
      cache['outputs'] = []
      
      cache['inputs'].extend(inputs)
      cache['outputs'].extend(outputs)

      cache['stdout'] = []
      cache['stderr'] = []

      cache['success'] = True
      for cmd in cmds:
        print('      Executing: ' + ' '.join(cmd))

        sub_env = os.environ.copy()
        sub_env["SLT_ENG_DIR"] = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../..')
        
        result = proc.run(cmd, stdout=proc.PIPE, stderr=proc.PIPE, env=sub_env)

        out_strings = list(filter(None, result.stdout.decode('utf-8').split('\n')))
        err_strings = list(filter(None, result.stderr.decode('utf-8').split('\n')))
        cache['stdout'].append(out_strings)
        cache['stderr'].append(err_strings)
        cache['success'] &= result.returncode == 0

        # Extract read/write files.
        for line in itertools.chain(out_strings, err_strings):
          read_file = line.split('Reading from file: ', 1)
          if len(read_file) == 2:
            cache['inputs'].append(read_file[1].rstrip())
          write_file = line.split('Writing to file: ', 1)
          if len(write_file) == 2:
            cache['outputs'].append(write_file[1].rstrip())

        if result.returncode != 0:
          print("Failure")

      cache['inputs'] = list(set(cache['inputs']))
      cache['outputs'] = list(set(cache['outputs']))
      with open(cache_file, 'w') as outfile:
        json.dump(cache, outfile, sort_keys=True, indent=2)


  def Build(self, tool, args):
    cmd = [tool] + _ArgsDictToList(args)
    self.Exec([cmd], [tool], [])

def main():
  dirname = os.path.dirname(os.path.abspath(__file__))

  args_parser = argparse.ArgumentParser(description='Runs a standard slt assets build')
  args_parser.add_argument('--dst', help='location of build destination', required=True)
  args_parser.add_argument('--src', help='The asset registry to build', required=True)

  args = args_parser.parse_args()

  ref_path = os.path.dirname(os.path.abspath(args.src))
  runner = Runner("_cache", ref_path, args.dst)

  with open(args.src) as data_file:
    registry_cfg = json.load(data_file)
    for k in registry_cfg.keys():
      rule = rules[k]
      tasks = registry_cfg[k]
      for name in tasks.keys():
        rule(runner, name, tasks[name])

  print('Build Complete')
if __name__ == "__main__":
  main()
