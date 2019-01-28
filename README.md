Sencor SWS 270 weather station sensor
===

Used a RXB6 like 433 Mhz receiver to capture informations, USB folder contains a simple C code to read USB serial output.

Packet specification:
* 4 bit unknown
* 8 bit unknown
* 4 bit 0000 = positive temp, 1111 = negative temp
* 8 bit temperature
* 4 bit unknown, always 1111
* 8 bit humidity

Timing is 480us
* 0 bit = T + 2T
* 1 bit = T + 4T
* space = 8T

RTL-SDR captured signal:
```
10010000100001001001001001000010000100100100100100100100100001001000010000100001000010010010000100001000010000100100100001000010010000100100100000000
```

RAW timing values producted by arduino:
```
3896 480 964 528 1936 492 1972 480 976 516 952 488 984 488 980 488 1944 496 1944 492 236 24 716 504 960 524 940 500 988 500 956 480 980 496 988 504 952 476 1960 488 988 488 1960 484 1008 448 1968 492 968 480 992 476 1960 504 1936 496 1952 556 1884 504 984 464 1968 504 952 512 1964 456 1936 532 960 496 1932 632 1836 484 
3896 524 948 496 1944 508 1956 536 892 532 956 528 944 556 928 480 1940 496 1960 520 916 508 980 496 988 492 984 456 1012 456 996 528 928 500 972 488 1952 512 968 472 1976 476 1004 460 1964 492 988 460 1004 472 1968 484 1944 496 1944 496 1948 516 956 500 468 40 1432 504 964 488 1352 164 424 504 1960 480 980 492 1964 484 1976 472 
3884 504 968 492 1964 480 1960 500 952 504 972 500 984 476 984 480 1952 492 1948 512 960 500 980 468 988 480 984 492 956 504 976 496 980 480 1000 456 1980 488 960 500 1956 476 1008 480 1940 496 980 484 1000 468 1944 504 1952 492 1940 492 1980 464 984 488 1952 500 972 484 1944 516 1944 484 988 484 1956 480 1972 460 
3884 528 952 492 1932 508 1948 508 972 504 952 512 968 488 960 512 1960 480 1948 480 984 504 960 508 984 464 984 492 988 484 972 488 968 500 960 508 1972 472 968 492 1972 464 984 488 1960 476 988 488 992 468 1964 488 1960 512 1928 488 1960 496 948 504 1960 492 960 500 1952 500 1940 492 972 504 1964 472 1960 496 
3900 508 952 492 1952 504 1952 488 976 496 960 512 976 488 972 484 1944 504 1956 480 976 500 968 500 976 484 984 504 928 528 968 492 968 508 968 492 1944 484 992 492 1948 488 964 508 1948 492 972 496 968 488 1968 496 1952 476 1952 488 1968 488 972 488 1976 472 976 492 1944 516 1932 496 964 508 1944 488 1964 496 
3892 524 952 500 1952 476 1960 500 976 480 980 492 964 508 972 512 1924 492 1960 496 952 520 960 504 960 492 972 492 996 472 992 472 984 488 964 496 1968 484 988 476 1948 500 972 492 1964 484 972 492 984 480 1956 492 1948 512 1924 496 1956 488 980 488 1964 488 972 492 1952 492 1972 472 960 516 1948 484 1964 492 
```

Sample bit stream is:
	0110 00011000 0000 01011001 1111 01011010
	0110 00011000 1111 11111010 1111 01010100
