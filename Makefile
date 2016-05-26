#----------------------------------------------------------
#编译工具用g++，以同时支持C和C++程序，以及二者的混合编译
CC=g++ -g -Wall
LDLIBS = -lm
Program = main

Bin= bin/
SrcDir= src/
ObjDir= obj/


#使用$(winldcard *.c)来获取工作目录下的所有.c文件的列表
#sources:=main.cpp command.c

#变量sources得到当前目录下待编译的.c/.cpp文件的列表，两次调用winldcard、结果连在一起即可
sources:=$(wildcard $(SrcDir)*.c) $(wildcard $(SrcDir)*.cpp)
#sources:=$(notdir $(fils))

#变量objects得到待生成的.o文件的列表，把sources中每个文件的扩展名换成.o即可。这里两次调用patsubst函数，第1次把sources中所有.cpp换成.o，第2次把第1次结果里所有.c换成.o
objects:=$(patsubst $(SrcDir)%,$(ObjDir)%,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(sources))))

#变量dependence得到待生成的.d文件的列表，把objects中每个扩展名.o换成.d即可。也可写成$(patsubst %.o,%.d,$(objects))
dependence:=$(objects:.o=.d)

#----------------------------------------------------------
#当$(objects)列表里所有文件都生成后，便可调用这里的 $(CC) $^ -o $@ 命令生成最终目标all了
#把all定义成第1个规则，使得可以把make all命令简写成make
$(Bin)$(Program): $(objects)
	$(CC) $(CPPFLAGS) $^ -o $@ $(LDLIBS)

#这段使用make的模式规则，指示如何由.c文件生成.o，即对每个.c文件，调用gcc -c XX.c -o XX.o命令生成对应的.o文件
#如果不写这段也可以，因为make的隐含规则可以起到同样的效果
$(ObjDir)%.o: $(SrcDir)%.c
	$(CC) $(CPPFLAGS) -c $< -o $@

#同上，指示如何由.cpp生成.o，可省略
$(ObjDir)%.o: $(SrcDir)%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

#因为这4行命令要多次凋用，定义成命令包以简化书写
define gen_dep
set -e;\
$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
sed 's,\($*\)\.o[ :]*,$(ObjDir)\1.o $@ : ,g' < $@.$$$$ > $@; \
rm -f $@.$$$$
endef

#指示如何由.c生成其依赖规则文件.d
#这段使用make的模式规则，指示对每个.c文件，如何生成其依赖规则文件.d，调用上面的命令包即可
$(ObjDir)%.d: $(SrcDir)%.c
	$(gen_dep)

#同上，指示对每个.cpp，如何生成其依赖规则文件.d
$(ObjDir)%.d: $(SrcDir)%.cpp
	$(gen_dep)
	
#----------------------------------------------------------
include $(dependence)	#注意该句要放在终极目标all的规则之后，否则.d文件里的规则会被误当作终极规则了	

#----------------------------------------------------------
#清除所有临时文件（所有.o和.d）。之所以把clean定义成伪目标，是因为这个目标并不对应实际的文件
.PHONY: clean
clean:	#.$$已在每次使用后立即删除。-f参数表示被删文件不存在时不报错
	rm -f all $(objects) $(dependence) $(Bin)$(Program)

echo:	#调试时显示一些变量的值
	@echo SrcDir=$(SrcDir)
	@echo ObjDir=$(ObjDir)
	@echo sources=$(sources)
	@echo objects=$(objects)
	@echo dependence=$(dependence)
	@echo CPPFLAGS=$(CPPFLAGS)

#提醒：当混合编译.c/.cpp时，为了能够在C++程序里调用C函数，必须把每一个要调用的C函数，其声明都包括在extern "C"{}块里面，这样C++链接时才能成功链接它们。
