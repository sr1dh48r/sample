#!/bin/python3

def ss_high(word):
    ss = int(list(word)[6])
    return (ss == 1)

def clk_on_toggle(word):
    global g_clk
    prev = g_clk
    g_clk = int(int(list(word)[7]) == 1)
    return ((prev == 0) and (g_clk == 1))

def bits_0(word):
    bit = int(((int(list(word)[5], 16) & 2**0)) == 1)
    return str(bit)

def bits_04(word):
    bits = format(int(list(word)[5], base=16), '04b')
    return str(bits)

def bits(word):
    if(g_wrc == 1): return bits_0(word)
    if(g_wrc == 4): return bits_04(word)

def nibble_to_hex(s):
    global g_bin
    g_bin = []
    g_hex.append('{:x}'.format(int(s, 2)))

# main

cap = "00000200 00000a01 00000200 00000a01 00000200 00000a01 00000200 00000a01 00000200 00000a01 00000200 00000a01 00000200 00000a01 00000200 00000a01"

with open("cap2.txt", "r+") as f: cap = f.read()

g_clk = 0
g_wrc = 1

g_bin = []
g_hex = []

for word in cap.split(' '):
    if(ss_high(word))       : continue
    if(clk_on_toggle(word)) : g_bin.append(bits(word))
    if(len(g_bin) == 4)     : nibble_to_hex("".join(g_bin))

    # quad mode : <op-code:1wire> <addr,4byte:4wire>
    if(g_wrc == 1 and "".join(g_hex[-2:]) == "38"):     # PP4IO
        g_wrc = 4;                                      # switch to 4 wire mode for
        g_bin = [];                                     # clear binary capture
        g_hex = ["".join(g_hex[-2:])]                   # record opcode

print(" ".join(g_hex))
