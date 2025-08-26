.global _start
_start:
  mov W0, #2
  mov W1, #2
  mov W2, #2
  mov W3, #2
  mul W4, W2, W3
  mul W2, W1, W4
  mul W1, W0, W2
  mov W0, #2
  mul W2, W1, W0
  mov W0, #1
  mov W1, #1
  mov W3, #3
  mov W4, #1
  mov W5, #2
  mul W6, W4, W5
  mul W4, W3, W6
  mul W3, W1, W4
  mul W1, W0, W3
  mov W0, #4
  add W3, W2, W1
  add W1, W3, W0
  sxtw X0, W1
  mov X16, #1
  svc #0x80
