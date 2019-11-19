arg = table.pack(...)

if arg["n"] == 0 then
	print("usage: [file1] [file2] [..]");
	return
end
if arg[1] == "--help" then
	print("converts header files with function definitions or c files into dynamic libraries");
	print("usage: [file1] [file2] [..]");
	return
end


functionregex="([\r\n]([%a%*])+%s+([%a%*])+%((^[\r\n])%)[\r\n])" -- word spaces word 

f = assert(io.open(arg[1],"r"))
text = f:read("*all");
io.close(f);

it = string.gmatch(text,functionregex)



for typ, name, args in it do
	print(typ,name,args)
end





