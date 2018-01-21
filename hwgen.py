with open('hw.bf', 'w') as f:
    for c in 'Hello, World!':
        f.write('+'*ord(c))
        f.write('.>')

with open('h.bf', 'w') as f:
    f.write('+'*ord('h') + '.')
