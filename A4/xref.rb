require 'open3'

# Get args

if ARGV.length < 1
    puts "Error: Missing executable name. "
end

exec_name = ARGV[0]

# Set regex for objdump and dwarfdump

obj_reg= /^([0-9a-f]+ <(\S+)>:)|^  ([0-9a-f]+):\t*((?: *[0-9a-f]{2})+) *\t([a-z]+) *([a-zA-Z0-9$,\%_\-\(\)\# \*\<\>\.\:\@\+\/\\]*)/

# Get results from objdump and dwarfdump

obj_raw, stdeerr, status = Open3.capture3("objdump -d " + exec_name)
File.open("outputs/objdumpd.txt", 'w') { |f| f.write(obj_raw) }
dwarf_raw, stdeerr, status = Open3.capture3("llvm-dwarfdump --debug-line " + exec_name)
File.open("outputs/dwarfdump.txt", 'w') { |f| f.write(dwarf_raw) }

obj = obj_raw.scan(obj_reg)
File.open("outputs/obj.txt", 'w') { |f| f.write(obj) }
