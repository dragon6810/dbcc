.global _start
_start:
  sub sp, sp, #120
  mov W0, #1
  mov W1, #1
  mov W2, #1
  mov W3, #1
  mov W4, #1
  mov W5, #1
  mov W6, #1
  mov W7, #1
  mov W8, #1
  mov W9, #1
  mov W10, #1
  mov W11, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  mov W12, #1
  add W12, W0, W1
  add W0, W2, W3
  add W1, W12, W0
  add W0, W4, W5
  add W2, W1, W0
  add W0, W6, W7
  add W1, W2, W0
  add W0, W8, W9
  add W2, W1, W0
  add W0, W10, W11
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  add W0, W12, W12
  add W1, W2, W0
  add W0, W12, W12
  add W2, W1, W0
  sxtw X0, W2
  mov X16, #1
  svc #0x80
  add sp, sp, #120
