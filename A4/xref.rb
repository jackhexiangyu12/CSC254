require 'open3'

# Get args

if ARGV.length < 1
    puts "Error: Missing executable name. "
end

exec_name = ARGV[0]

# Set regex for objdump and dwarfdump

# Get results from objdump and dwarfdump

obj_raw, stdeerr, status = Open3.capture3("objdump -d " + exec_name)
File.open("examples/objdump.txt", 'w') { |f| f.write(obj_raw) }
dwarf_raw, stdeerr, status = Open3.capture3("llvm-dwarfdump --debug-line " + exec_name)
File.open("examples/dwarfdump.txt", 'w') { |f| f.write(dwarf_raw) }
