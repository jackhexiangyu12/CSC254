require 'open3'
require "erb"

# Get args

if ARGV.length < 1
    abort("Error: Missing executable name. ")
end

exec_name = ARGV[0]

# By assembly or by source

# Set regex for objdump and dwarfdump

obj_reg =   /^(?<first>[0-9a-f]+ <(?<name>\S+)>:)|^  (?<addr>[0-9a-f]+:\t*(?: *[0-9a-f]{2})+ *\t[a-z]+ *[a-zA-Z0-9$,\%_\-\(\)\# \*\<\>\.\:\@\+\/\\]*)/
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
        assembly_map[l[2].to_i(16)] = l[2..5]
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
                file_content.push(["<span class=\"nocode\">"+line_num.to_s+"</span>", line, false])
            }
            file_all[file_name] = file_content
        end
    end
}

# Sort dwarf table

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
    for i in 0..table.length-2
        addr = table[i][0].to_i(16)
        code_map_asm[addr] = {}
    end
}

addr_map.each { |key, table|

    for i in 0..table.length-2
        addr_start = table[i][0].to_i(16)
        addr_end = table[i+1][0].to_i(16) - 1

        line_end = table[i][1].to_i(10)
        line_start = 1

        curr_index = addr_map_sorted[key].index(line_end)
        if curr_index != 0
            line_start = addr_map_sorted[key][curr_index-1]+1
        end

        file_num = table[i][3].to_i(10)
        file_name = file_map[key][file_num]

        if code_map_asm[addr_start].key?('asm')
            asm_code = code_map_asm[addr_start]['asm']
            src_code = code_map_asm[addr_start]['src']
            if src_code[-1] != nil && src_code[-1][1] != nil
                src_code[-1][1] = src_code[-1][1] + "\n"
            end
    
            if asm_code[-1] != nil && asm_code[-1][-1] != nil
                asm_code[-1][-1] = asm_code[-1][-1] + "\n"
            end
        else
            asm_code = Array.new
            src_code = Array.new
        end
        
        offset = line_end - line_start
        if offset > 0
            for j in line_start..line_end - 1
                asm_code << [" ", "\n"]
            end
        end

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

        for j in line_start..line_end

            src_code << [file_all[file_name][j][0], file_all[file_name][j][1], file_all[file_name][j][2]]
            
            if !file_all[file_name][j][2]
                file_all[file_name][j][2] = true
            end
        end

        if src_code[-1] != nil && src_code[-1][1] != nil
            src_code[-1][1] = src_code[-1][1].sub("\n", "")
        end

        if asm_code[-1] != nil && asm_code[-1][-1] != nil
            asm_code[-1][-1] = asm_code[-1][-1].sub("\n", "")
        end

        content = {
            "asm" => asm_code.compact,
            "file_name" => file_name,
            "src" => src_code.compact
        }

        code_map_asm[addr_start] = content
    end
}

# Generate code block content

# puts(file_all)
# puts("=" * 50)
# puts(assembly_map)
# puts("=" * 50)
# puts(addr_map)
# puts("=" * 50)
# puts(file_map)
# puts("=" * 50)
# puts(code_map_asm)

# By source file

# code_map_src = Hash.new

# addr_map.each { |key, table|
#     for i in 0..table.length-2
#         addr_1 = table[i][0].to_i(16)
#         addr_2 = table[i+1][0].to_i(16)

#         assembly_list = Array.new
#         for j in addr_1..addr_2-1
#             assembly_list.push(assembly_map[j])
#         end

#         file_num = table[i][3]
#         file_name = file_map[key][file_num.to_i]

#     end
# }


# Generate HTML

# Build template data class

class Product
    def initialize(executable)
        @exec_name = executable
        @code_blocks = [ ]
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
<% if !src[2] %><%= src[0]+". "+src[1] %><% else %><span class="nocode"><font color="grey"><%= src[0] + ". " + src[1] %></font></span><% end %>
<% end %></pre>
            </div>
        </div>
    <% end %>
    </body>
    </html>
}.gsub(/^  /, '')

rhtml = ERB.new(template, 0, '>')
cross_indexor = Product.new(exec_name)

code_map_asm.each { |addr, content|
    cross_indexor.add_code_block(content['asm'], content['src'], content['file_name'])
}

out = rhtml.result(cross_indexor.get_binding)
File.open("index.html", 'w') { |f| f.write(out) }