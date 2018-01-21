import os, sys

p = os.popen('./bfjit tests/hw.bf')
if p.read() != 'Hello, World!':
    sys.exit(1)

p.close()
p = os.popen('./bfjit tests/fz.bf')
bf_fz = p.read()
p.close()
p = os.popen('python3 tests/fz.py')
py_fz = p.read()
p.close()

if py_fz != bf_fz:
    sys.exit(1)

p = os.popen('./bfjit tests/hwcomplex.bf')
s = p.read()
p.close()

if s != 'Hello World!\n':
    sys.exit(1)
