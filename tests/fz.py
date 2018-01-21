# fizzbuzz in python

for i in range(1, 101):
    printed = False
    if i % 3 == 0:
        printed = True
        print('Fizz', end='')
    if i % 5 == 0:
        printed = True
        print('Buzz', end='')
    if not printed:
        print(i, end='')
    print()
