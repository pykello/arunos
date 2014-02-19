set $lastcs = -1

echo + target remote localhost:26000\n
target remote localhost:26000

echo + symbol-file arunos.elf\n
add-symbol-file ./arunos.elf 0x80010000
