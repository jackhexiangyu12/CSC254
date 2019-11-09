require 'open3'
require "erb"

# Get args

if ARGV.length < 1
    abort("Error: Missing executable name. ")
end

exec_name = ARGV[0]

# Set regex for objdump and dwarfdump

obj_func_reg = /^([0-9a-f]+) <(\S+)>:/
obj_reg = /^(?<first>[0-9a-f]+ <(?<name>\S+)>:)|^  (?<addr>[0-9a-f]+:\t*(?: *[0-9a-f]{2})+ *\t[a-z]+ *[a-zA-Z0-9$,\%_\-\(\)\# \*\<\>\.\:\@\+\/\\]*)/
dwarf_file_reg = /^(?<first>debug_line\[[0-9a-fx]+\])|^file_names\[ *(?<num>\d+)\]:\n +name: \"(?<name>.+\.[ch]{1})\"/
dwarf_line_reg = /^(?<first>debug_line\[[0-9a-fx]+\])|^(?<addr>0x[0-9a-f]+) +(?<line>\d+) +(?<col>\d+) +(?<file>\d+) +(?<isa>\d+) +(?<dis>\d+) +(?<flags>.+)/

# Get results from objdump and dwarfdump

obj_raw, stdeerr, status = Open3.capture3("objdump -d " + exec_name)
File.open("objdumpd.txt", 'w') { |f| f.write(obj_raw) }

dwarf_raw, stdeerr, status = Open3.capture3("llvm-dwarfdump --debug-line " + exec_name)
File.open("dwarfdump.txt", 'w') { |f| f.write(dwarf_raw) }

obj = obj_raw.scan(obj_reg)
func = obj_raw.scan(obj_func_reg)
dwarf_file = dwarf_raw.scan(dwarf_file_reg)
dwarf_line = dwarf_raw.scan(dwarf_line_reg)

# File.open("outputs/obj.txt", 'w') { |f| f.write(obj) }
# File.open("outputs/file.txt", 'w') { |f| f.write(dwarf_file) }
# File.open("outputs/line.txt", 'w') { |f| f.write(dwarf_line) }

# Store assembly code as a hash map using addr as key

assembly_map = Hash.new

obj.each { |l|
    if l[2] != nil
        assembly_map[l[2].to_i(16)] = l[2..5]
    end
}

func_map = Hash.new

func.each { |l|
    func_map[l[1]] = l[0].to_i(16).to_s(16)
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

# Store all source files

file_all = Hash.new

addr_map.each { |key, table|
    for i in 0..table.length-1
        file_num = table[i][3]
        file_name = file_map[key][file_num.to_i(10)]

        if !file_all.key?(file_name)
            file_content = Array.new
            file_content.push("")
            File.foreach(file_name).with_index { |line, line_num|
                file_content.push(["<span class=\"nocode\">"+(line_num+1).to_s+"</span>", line, false])
            }
            file_all[file_name] = file_content
        end
    end
}

# Sort line numbers in each dwarf table and store

addr_map_sorted = Hash.new

addr_map.each { |key, table|
    table_sorted = table.sort_by{ |line| line[1].to_i(10)}
    
    line_sorted = Array.new
    table_sorted.each{ |line| line_sorted.push(line[1].to_i(10))}

    addr_map_sorted[key] = line_sorted
}

# By assenbly file

code_map_asm = Hash.new

addr_map.each { |key, table|

    for i in 0..table.length-1
        if !table[i][6].include?("stmt")
            next
        end

        if table[i][6].include?("end_sequence")
            next
        end

        # Range in assembly address
        addr_start = table[i][0].to_i(16)
        addr_end = table[i+1][0].to_i(16) - 1

        # Range in source line number
        line_end = table[i][1].to_i(10)
        line_start = 1
        curr_index = addr_map_sorted[key].index(line_end)
        if curr_index != 0
            line_start = addr_map_sorted[key][curr_index-1]+1
        end

        # Get file name
        file_num = table[i][3].to_i(10)
        file_name = file_map[key][file_num]

        # Add to hash map
        if code_map_asm[addr_start]
            asm_code = code_map_asm[addr_start]['asm']
            src_code = code_map_asm[addr_start]['src']
        else
            asm_code = Array.new
            src_code = Array.new
        end

        # Append new line if needed
        offset = line_end - line_start
        if offset > 0
            for j in line_start..line_end - 1
                asm_code << [" ", "\n"]
            end
        end

        # Add href
        for j in addr_start..addr_end
            if assembly_map[j] != nil
                asm_tmp = assembly_map[j][0]
                asm_tmp_split = asm_tmp.split(" ")
                if asm_tmp_split[-1].include?("<")
                    asm_tmp_called_addr = asm_tmp_split[-2]
                    asm_tmp = asm_tmp.sub(asm_tmp_called_addr, "<span class=\"nocode\"><a href=#asmline"+asm_tmp_called_addr+">"+asm_tmp_called_addr+"</a></span>")
                end

                asm_code << [j.to_s(16), asm_tmp + "\n", ]
            end
        end

        # Check if current src line is used or not
        for j in line_start..line_end
            src_code << [file_all[file_name][j][0], file_all[file_name][j][1], file_all[file_name][j][2]]
            
            if !file_all[file_name][j][2]
                file_all[file_name][j][2] = true
            end
        end

        content = {
            "asm" => asm_code.compact,
            "file_name" => file_name,
            "src" => src_code.compact
        }
        code_map_asm[addr_start] = content
    end
}

# Generate HTML

# Build template data class

class Product
    def initialize(executable, func_map)
        @exec_name = executable
        @code_blocks = [ ]
        @func_map = func_map
    end

    def add_code_block(asm, src, file_name)
        code_block = {
            'asm' => asm,
            'src' => src,
            'file_name' => file_name
        }
        @code_blocks << code_block
    end

    # Support templating of member data.
    def get_binding
        binding
    end
end

template = %{
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>xref for binary: <%= @exec_name %></title>
        <script src="https://cdn.jsdelivr.net/gh/google/code-prettify@master/loader/run_prettify.js"></script>
        <style>
            .code-block {
                display: table;
                box-sizing: border-box;
                position: relative;
                table-layout: fixed;
                width: 100%;
            }
            pre {
                margin: 0;
                overflow: auto;
                white-space: pre-wrap;
                word-wrap: break-word;
            }
            .asm-block,
            .src-block {
                width: 50%;
                display: table-cell;
                vertical-align: top;
            }
            li.L0, li.L1, li.L2, li.L3,
            li.L5, li.L6, li.L7, li.L8 {
                list-style-type: decimal !important;
            }
        </style>
    </head>
    <body>
    <h2>xref for binary: <%= @exec_name %> </h2>
    <a href=<%= "#asmline" + @func_map["main"] %>>main</a>
    <% @code_blocks.each do |code_block| %>
        <div class="code-block">
            <div class="asm-block">
                <span>~</span>
                <pre class="prettyprint lang-c">
<% code_block['asm'].each do |asm| %>
<a name=<%= "asmline" + asm[0] %> href=<%= "#asmline" + asm[0] %>><%= asm[1].sub('\t', '</a>') %>
<%end%></pre>
            </div>
            <div class="src-block">
                <span><%= code_block['file_name'] %></span>
                <pre class="prettyprint lang-c">
<% code_block['src'].each do |src| %>
<% if !src[2] %><%= src[0]+". "+src[1] %><% else %><span class="nocode"><font color="grey"><%= src[0] + ". " + src[1].sub("\n", "")%></font></span>\n<% end %>
<% end %></pre>
            </div>
        </div>
    <% end %>
    </body>
    </html>
}.gsub(/^  /, '')

rhtml = ERB.new(template, 0, '>')
cross_indexor = Product.new(exec_name, func_map)

code_map_asm.each { |addr, content|
    cross_indexor.add_code_block(content['asm'], content['src'], content['file_name'])
}

out = rhtml.result(cross_indexor.get_binding)
File.open("HTML/index.html", 'w') { |f| f.write(out) }