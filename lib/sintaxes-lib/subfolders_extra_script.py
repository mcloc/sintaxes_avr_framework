Import('env')
from os.path import realpath

# private library flags
for item in env.get("CPPDEFINES", []):
        env.Append(CPPPATH=[realpath("sintaxes-lib")])
        env.Append(CPPPATH=[realpath("sintaxes-lib/msgpack")])
        env.Append(CPPPATH=[realpath("sintaxes-lib/devices")])
        env.Append(CPPPATH=[realpath("sintaxes-lib/devices/types")])
        env.Append(CPPPATH=[realpath("sintaxes-lib/defines")])
        env.Append(CPPPATH=[realpath("sintaxes-lib/4BCProtocol")])
        env.Append(SRC_FILTER=["+<*>"])
        break

