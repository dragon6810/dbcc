.global _start
_start:
  mov X0, #42
  mov X16, #1
  svc #0x80
