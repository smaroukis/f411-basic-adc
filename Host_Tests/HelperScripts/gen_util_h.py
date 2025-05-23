#!/usr/bin/env python3

import sys
import re

if len(sys.argv) != 3:
    print("Usage: python3 gen_util_h.py <source_file.c> <header_output.h>")
    sys.exit(1)

src_file = sys.argv[1]
header_file = sys.argv[2]

pattern = re.compile(r'^\s*(?!static)(\w[\w\s\*]+)\s+(\w+)\s*\(([^)]*)\)\s*\{', re.MULTILINE)

with open(src_file, 'r') as f:
    source = f.read()

matches = pattern.findall(source)

with open(header_file, 'w') as f:
    f.write("#ifndef UTIL_H\n#define UTIL_H\n\n")
    for ret_type, name, args in matches:
        args = args.strip() or "void"
        f.write(f"{ret_type.strip()} {name}({args});\n")
    f.write("\n#endif // UTIL_H\n")

print(f"✅ Header generated: {header_file}")
