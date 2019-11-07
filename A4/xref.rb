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

assembly_map = Hash.new

obj.each { |l|
    if l[2] != nil
        assembly_map[l[2].to_i(16)] = [l[2..5]]
    end
}

# Extract file names in dwarf

file_map = Hash.new
list = Array.new
curr = nil

dwarf_file.each { |l|
    if l[0] != nil
        if curr != nil
            file_map[curr] = list
        end
        curr = l[0]
        list = Array.new
        list.push("")
    else
        list.push(l[2])
    end
}
file_map[curr] = list

# Extract the table in dwarf

addr_map = Hash.new
list = Array.new
curr = nil

dwarf_line.each { |l|
    if l[0] != nil
        if curr != nil
            addr_map[curr] = list
        end
        curr = l[0]
        list = Array.new
    else
        list.push(l[1..7])
    end
}
addr_map[curr] = list

#

code_map = Hash.new

addr_map.each { |key, table|
    for i in 0..table.length-2
        addr_1 = table[i][0].to_i(16)
        addr_2 = table[i+1][0].to_i(16)
        
        assembly_list = Array.new
        for j in addr_1..addr_2-1
            assembly_list.push(assembly_map[j])
        end

        file_num = table[i][3]
        file_name = file_map[key][file_num.to_i]

        
    end

}

# Generate HTML
