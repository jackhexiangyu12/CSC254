require 'open3'
require "erb"

# Get args

if ARGV.length < 1
    abort("Error: Missing executable name. ")
end

exec_name = ARGV[0]

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

# Store all source files

file_all = Hash.new

addr_map.each { |key, table|
    for i in 0..table.length-1
        file_num = table[i][3]
        file_name = file_map[key][file_num.to_i]

        if !file_all.key?(file_name)
            file_content = Array.new
            File.foreach(file_name).with_index { |line, line_num|
                file_content.push(line)
            }
            file_all[file_name] = file_content
        end
       
    end
}



# By asm file

code_map = Hash.new

addr_map.each { |key, table|

    for i in 0..table.length-2
        addr_start = table[i][0].to_i(16)
        addr_end = table[i+1][0].to_i(16) - 1

        line = table[i][1].to_i(10)

        file_num = table[i][3].to_i(10)
        file_name = file_map[key][file_num]

        asm_code = Array.new
        for i in addr_start..addr_end
            # puts(i, assembly_map[i])
            if assembly_map[i] != nil
                asm_code << [i.to_s(16), assembly_map[i][0][0]]
            end
        end

        content = {
            "asm" => asm_code.compact,
            "file_name" => file_name,
            "file_num" => file_num,
            "line_num" => line + 1,
            "src" => file_all[file_name][line]
        }

        code_map[addr_start] = content
    end

}

# Generate code block content

puts(file_all)
puts("=" * 50)
puts(assembly_map)
puts("=" * 50)
puts(addr_map)
puts("=" * 50)
puts(file_map)
puts("=" * 50)
puts(code_map)


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
        puts(code_block)
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
                <pre class="prettyprint lang-c linenums:1">
                    <% code_block['asm'].each do |asm| %>
                        <a name=<%= "asmline" + asm[0] %> href=<%= "#asmline" + asm[0] %> ><%= asm[1].sub('\t', '</a>') %>
                    <%end%> 
                    </pre>   
            </div>
            <div class="src-block">
                <span><%= code_block['file_name'] %></span>
                <pre class="prettyprint lang-c linenums:1">
                    <%= code_block['src'] %></pre>
            </div>
        </div>
    <% end %>
    </body>
    </html>
}.gsub(/^  /, '')


rhtml = ERB.new(template)
cross_indexor = Product.new(exec_name)

code_map.each { |addr, content|
    cross_indexor.add_code_block(content['asm'], content['src'], content['file_name'])
}

# rhtml.run(cross_indexor.get_binding)
out = rhtml.result(cross_indexor.get_binding)
# puts(out)
File.open("outputs/index.html", 'w') { |f| f.write(out) }




