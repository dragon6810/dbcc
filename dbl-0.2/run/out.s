.global _start
_start:
  mov W0, #2
  mov W1, #2
  mov W2, #2
  mov W3, #2
  mov W4, #2
  mul W5, W3, W4
  mul W6, W2, W5
  mul W7, W1, W6
  mul W8, W0, W7
  mov W9, #1
  mov W10, #1
  mov W11, #3
  mov W12, #1
  mov W13, #2
  mul W14, W12, W13
  mul W15, W11, W14
  mul W16, W10, W15
  mul W17, W9, W16
  add W18, W8, W17
  sxtw X0, W18
  mov X16, #1
  svc #0x80
