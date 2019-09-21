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
      "-ffat-lto-objects"
  ]
)
