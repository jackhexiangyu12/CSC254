require 'open3'

# Get args

if ARGV.length < 1
    abort("Error: Missing executable name. ")
end

exec_name = ARGV[0]

# Set regex for objdump and dwarfdump

obj_reg = /^(?<first>[0-9a-f]+ <(?<name>\S+)>:)|^  (?<addr>[0-9a-f]+):\t*(?<sth>(?: *[0-9a-f]{2})+) *\t(?<func>[a-z]+) *(?<args>[a-zA-Z0-9$,\%_\-\(\)\# \*\<\>\.\:\@\+\/\\]*)/
dwarf_file_reg = /^(?<first>debug_line\[[0-9a-fx]+\])|^file_names\[ *(?<num>\d+)\]:\n +name: \"(?<name>.+\.[ch]{1})\"/
dwarf_line_reg = /^(?<first>debug_line\[[0-9a-fx]+\])|^(?<addr>0x[0-9a-f]+) +(?<line>\d+) +(?<col>\d+) +(?<file>\d+) +(?<isa>\d+) +(?<dis>\d+) +(?<flags>.+)/

# Get results from objdump and dwarfdump

obj_raw, stdeerr, status = Open3.capture3("objdump -d " + exec_name)
File.open("outputs/objdumpd.txt", 'w') { |f| f.write(obj_raw) }

dwarf_raw, stdeerr, status = Open3.capture3("llvm-dwarfdump --debug-line " + exec_name)
File.open("outputs/dwarfdump.txt", 'w') { |f| f.write(dwarf_raw) }

obj = obj_raw.scan(obj_reg)
dwarf_file = dwarf_raw.scan(dwarf_file_reg)
dwarf_line = dwarf_raw.scan(dwarf_line_reg)

# File.open("outputs/obj.txt", 'w') { |f| f.write(obj) }
# File.open("outputs/file.txt", 'w') { |f| f.write(dwarf_file) }
# File.open("outputs/line.txt", 'w') { |f| f.write(dwarf_line) }

# Store assembly code as a hash map using addr as key

assembly = Hash.new

obj.each { |l|
    if l[2] != nil
        assembly[l[2]] = [l[3..5]]
    end
}

# Extract file names in dwarf

files = Hash.new
list = Array.new
curr = nil

dwarf_file.each { |l|
    if l[0] != nil
        if curr != nil
            files[curr] = list
        end
        curr = l[0]
        list = Array.new
        list.push("")
    else
        list.push(l[2])
    end
}
files[curr] = list

# Extract the table in dwarf

table = Hash.new
list = Array.new
curr = nil

dwarf_line.each { |l|
    if l[0] != nil
        if curr != nil
            table[curr] = list
        end
        curr = l[0]
        list = Array.new
    else
        list.push(l[1..7])
    end
}
table[curr] = list

# Store the map between assembly and source

map = Hash.new

table.each { |k, t|
    
    t.each { |l|

    }
}

# Generate HTML
