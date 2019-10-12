Import("env")

#
# Dump build environment (for debug)
# print(env.Dump())
#

env.Append(
  LINKFLAGS=[
      #"-static",
      "-lm",
      "-flto",
      "-fuse-linker-plugin",
      #"-static-libgcc",
      #"-static-libstdc++",
      "-ffat-lto-objects",
      #  "-I", "lib/sintaxes-lib",
#	"-I", "lib/sintaxes-lib/4BCProtocol",
#	"-I", "lib/sintaxes-lib/defines",
#	"-I", "lib/sintaxes-lib/msgpack",
#	"-I", "lib/sintaxes-lib/msgpack/type_values"
  ]
)
