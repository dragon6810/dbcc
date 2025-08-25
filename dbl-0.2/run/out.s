.global _start
_start:
  mov W0, #1
  mov W1, #2
  mul W2, W0, W1
  mov W3, #20
  mov W4, #2
  mul W5, W3, W4
  add W6, W2, W5
  sxtw X0, W6
  mov X16, #1
  svc #0x80
