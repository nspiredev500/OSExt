-- sorry, I don't know python and I know lua from OpenComputers and the nspire, so lua it is
--
arg = table.pack(...)

if arg["n"] <= 1 then
	print("usage: [file1] [file2] [..]");
	return
end
if arg[1] == "--help" then
	print("converts header files with function definitions or c files into dynamic libraries");
	print("usage: [libfile] [file1] [file2] [..]");
	print("options: --help display this help\n-q don't ask questions")
	return
end

libpro = "\n#include \"include/osext.h\"\nvoid main(int argsn, char* argc)\n{\n	assertOSExt();\n	initRecursiveDynlinker();\n"
libepi = "\n	return 0;\n}"


headerpro1 = "\n#include \"include/osext.h\"\n#ifndef "
headerpro2 = "\nvoid load"
headerpro3 = "()\n{\n	assertOSExt();\n	initRecursiveDynlinker();\n"
headerepi = "\n	return;\n}\n#endif"



--functionregex="([\r\n]([%a%*]+)%s+([%a%*]+)%s*%((^[\r\n])%)[\r\n])" -- word spaces word 
--functionregex="([\r\n]+([%a%*_]-)%s*([%a%*_]+)(%**)%s+(%**)([%a%*_]+)%s*(%([^\n]*%))(;?))" -- static word spaces word 
--functionregex="([\r\n]+([%a%*_]-)%s*([%a%*_]+)(%**)%s+(%**)([%a%*_]+)%s*(%([^\n]-%))(;?))(.-)[\r\n]+(%}?)"
functionregex="([\r\n]+([%a%*_]-)%s*([%a%*_]+)(%**)%s+(%**)([%a%*_]+)%s*(%([^\n]-%))(;?))(.-)[\r\n]-(%}?)"


endfile = nil
--f = assert(io.open(arg[1],"r"))
--text = f:read("*all");
--io.close(f);
text = ""
quiet = false;
for i,j in ipairs(arg) do
	if j == "-q" then
		quiet = true;
	else
	--print(endfile)
		if endfile == nil then
			endfile = j
		else
			local f = assert(io.open(j,"r"))
			text = text..f:read("*all");
			io.close(f)
		end
	end
end



it = string.gmatch(text,functionregex)

export = {}
names = {}
for full,static ,typ, stars1, stars2, name, args, semicolon, body, brace in it do
	if static == "return" or typ == "return" then
		
	else
		local found = false
		for i,j in ipairs(names) do
			if (j == name) then
				found = true
				break;
			end
		end
		if not found then
			--print(static,typ,name,args,semicolon)
			if typ ~= "__asm__" then
				if not quiet then
					print("export this function?: (y/n)");
				end
				print(static,typ,name,args,semicolon,body)
				if stars1 == nil then
					stars1 = ""
				end
				if stars2 == nil then
					starts2 = ""
				end
				if brace ~= "}" then
					body = nil
				end
				if quiet or io.stdin:read() == "y" then
					print("exporting")
					table.insert(export,{typ,name,args,stars1..stars2,body})
				end
			end
		end
		table.insert(names,name)
	end
end

dynlib = libpro
libinsert = ""
libinsert2 = "unsigned int table[] = {"
header = headerpro1..string.upper(arg[1]).."\n#define "..string.upper(arg[1]).."\n"
headerinsert = "	unsigned int *table = requestLibrary_r(\""..arg[1].."\");\n"

for i,j in ipairs(export) do
	header = header..j[1].." "..j[4].."(*"..j[2]..")"..j[3]..";\n"
	headerinsert = headerinsert.."	"..j[2].." = table+"..(i-1)..";\n"
	if j[5] ~= nil then
		libinsert = libinsert..header..j[1].." "..j[4]..j[2]..j[3]..j[5].."}\n"
	end
	libinsert2 = libinsert2..j[2]..","
end
libinsert2 = libinsert2.."0};\n"
dynlib = libinsert..libinsert2..dynlib.."	registerLibrary_r(\""..arg[1].."\",table);\n"..libepi
header = header..headerpro2..arg[1]..headerpro3..headerinsert..headerepi

print("Header: ")
print(header)
print("Library: ")
print(dynlib)

local f = io.open(arg[1]..".h","w")
f:write(header)
io.close(f)
f = io.open(arg[1]..".c","w")
f:write(dynlib)
io.close(f)












