import time

energy_level = 97
energy_health = 95

print("Hello")
while True:
    print("<data>")
    print(f'{int(energy_level)}C')
    print("<data>")
    print(f'{int(energy_health)}H')
    energy_level -= 0.2
    energy_health -= 0.04