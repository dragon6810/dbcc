.global _start
_start:
  mov X0, #41
  mov X1, #1
  add X2, X0, X1
  mov X0, X2
  mov X16, #1
  svc #0x80
