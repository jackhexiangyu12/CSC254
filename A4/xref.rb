require 'open3'

# Get args

if ARGV.length < 1
    abort("Error: Missing executable name. ")
end

exec_name = ARGV[0]

# Set regex for objdump and dwarfdump

obj_reg = /^([0-9a-f]+ <(\S+)>:)|^  ([0-9a-f]+):\t*((?: *[0-9a-f]{2})+) *\t([a-z]+) *([a-zA-Z0-9$,\%_\-\(\)\# \*\<\>\.\:\@\+\/\\]*)/
dwarf_file_reg = /^(?<first>debug_line\[[0-9a-fx]+\])|^file_names\[ *(?<num>\d+)\]:\n +name: \"(?<name>.+\.[ch]{1})\"/
dwarf_line_reg = /^(?<first>debug_line\[[0-9a-fx]+\])|^(?<address>0x[0-9a-f]+) +(?<line>\d+) +(?<col>\d+) +(?<file>\d+) +(?<isa>\d+) +(?<dis>\d+) +(?<flags>.+)/

# Get results from objdump and dwarfdump

obj_raw, stdeerr, status = Open3.capture3("objdump -d " + exec_name)
File.open("outputs/objdumpd.txt", 'w') { |f| f.write(obj_raw) }

dwarf_raw, stdeerr, status = Open3.capture3("llvm-dwarfdump --debug-line " + exec_name)
File.open("outputs/dwarfdump.txt", 'w') { |f| f.write(dwarf_raw) }

obj = obj_raw.scan(obj_reg)
File.open("outputs/obj.txt", 'w') { |f| f.write(obj) }

dwarf_file = dwarf_raw.scan(dwarf_file_reg)
File.open("outputs/file.txt", 'w') { |f| f.write(dwarf_file) }

dwarf_line = dwarf_raw.scan(dwarf_line_reg)
File.open("outputs/line.txt", 'w') { |f| f.write(dwarf_line) }

# Process

