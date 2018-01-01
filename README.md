# The SLT Engine

# Usage
## Build and install

This is the recommended way to use the engine, if you want an engine built in
RELEASE mode, yet still have the game code be debuggable.

```
mkdir slt_build
cd slt_build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=slt_install_location ../
make -j4
make install
```

Depending on your system setup, the `cmake` command might fail, asking to
provide additional arguments specifying where to find dependencies like glfw or
the fbx sdk, but the errors should provide simple instructions on how to fix
each one.
