.global _start
_start:
  mov X0, #41
  mov X0, #1
  mov X16, #1
  svc #0x80
