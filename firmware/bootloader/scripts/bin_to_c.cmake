# Convert a binary file to a C array.
# Usage: cmake -DINPUT=... -DOUTPUT=... -DSYMBOL=... -P bin_to_c.cmake

if(NOT DEFINED INPUT OR NOT DEFINED OUTPUT OR NOT DEFINED SYMBOL)
  message(FATAL_ERROR "INPUT, OUTPUT, and SYMBOL are required")
endif()

file(READ "${INPUT}" HEX HEX)
string(LENGTH "${HEX}" HEX_LEN)
math(EXPR NBYTES "${HEX_LEN} / 2")

set(OUT "/* Generated from ${INPUT} — do not edit. */\n")
string(APPEND OUT "#include <stdint.h>\n")
string(APPEND OUT "const uint8_t ${SYMBOL}[] = {\n")

set(I 0)
set(LINE "")
while(I LESS HEX_LEN)
  string(SUBSTRING "${HEX}" ${I} 2 BYTE)
  string(APPEND LINE "0x${BYTE},")
  math(EXPR I "${I} + 2")
  math(EXPR COL "${I} / 2")
  math(EXPR MOD "${COL} % 16")
  if(MOD EQUAL 0)
    string(APPEND OUT "  ${LINE}\n")
    set(LINE "")
  endif()
endwhile()
if(NOT LINE STREQUAL "")
  string(APPEND OUT "  ${LINE}\n")
endif()

string(APPEND OUT "};\n")
string(APPEND OUT "const unsigned ${SYMBOL}_len = ${NBYTES};\n")
file(WRITE "${OUTPUT}" "${OUT}")
