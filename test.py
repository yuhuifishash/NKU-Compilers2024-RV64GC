import argparse
import subprocess
import os

def execute(command):
    return subprocess.run(command, capture_output=True, text=True)

def execute_with_stdin_out(command):
    return os.system(command)%255

def check_file(file1,file2):
    result = execute(["diff","--strip-trailing-cr",file1,file2,"-b"])
    # print(result)
    return result.returncode

def add_returncode(file,ret):
    need_newline = False
    with open(file, "r") as f:
        content = f.read()
        if len(content) > 0:
            if not content.endswith("\n"):
                need_newline = True

    with open(file, "a+") as f:
        if need_newline:
            f.write("\n")
        f.write(str(ret))
        f.write("\n")
    return False

# 测试用例的返回值的不要为124或139，否则会导致测试程序误判

def execute_ir(input,output,opt,stdin,stdout,testout):
    result = execute(["timeout","10","./bin/SysYc","-llvm","-o",output,input,opt])
    if(result.returncode != 0):
        print("\033[93mCompile Error on \033[0m"+input)
        return 0
    
    result = execute(["clang-15",output,"-c","-o","tmp.o","-w"])
    if(result.returncode != 0):
        print("\033[93mOutPut Error on \033[0m"+input)
        return 0
        
    result = execute(["clang-15","-static","tmp.o","lib/libsysy_x86.a"])
    if(result.returncode != 0):
        result = execute(["rm","-rf","tmp.o"])
        print("\033[93mLink Error on \033[0m"+input)
        return 0
    
    execute(["rm","-rf","tmp.o"])
    result = 0
    if(stdin=="none"):
        result = execute_with_stdin_out("timeout 10 ./a.out > "+testout + " 2>/dev/null")
    else:
        result = execute_with_stdin_out("timeout 10 ./a.out < "+stdin+" > "+testout + " 2>/dev/null")
    if(result == 124):
        print("\033[93mTime Limit Exceed on \033[0m"+input)
        return 0
    elif(result == 139):
        print("\033[95mRunTime Error on \033[0m"+input)
        return 0
    
    add_returncode(testout,result)
    
    if(check_file(testout,stdout)==0):
        print("\033[92mAccept \033[0m"+input)
        return 1
    else:
        print("\033[91mWrong Answer on \033[0m"+input)
        return 0
   
def execute_asm(input,output,opt,stdin,stdout,testout):
    result = execute(["timeout","60","./bin/SysYc","-S","-o",output,input,opt])
    if(result.returncode != 0):
        print("\033[93mCompile Error on \033[0m"+input)
        return 0
    
    result = execute(["riscv64-unknown-linux-gnu-gcc",output,"-c","-o","tmp.o","-w"])
    if(result.returncode != 0):
        print("\033[93mOutPut Error on \033[0m"+input)
        return 0
        
    result = execute(["riscv64-unknown-linux-gnu-gcc","-static","tmp.o","lib/libsysy_rv.a"])
    if(result.returncode != 0):
        result = execute(["rm","-rf","tmp.o"])
        print("\033[93mLink Error on \033[0m"+input)
        return 0
    
    execute(["rm","-rf","tmp.o"])
    result = 0
    if(stdin=="none"):
        result = execute_with_stdin_out("timeout 20 qemu-riscv64 ./a.out > "+testout + " 2>/dev/null")
    else:
        result = execute_with_stdin_out("timeout 20 qemu-riscv64 ./a.out < "+stdin+" > "+testout + " 2>/dev/null")
    if(result == 124):
        print("\033[93mTime Limit Exceed on \033[0m"+input)
        return 0
    elif(result == 139):
        print("\033[95mRunTime Error on \033[0m"+input)
        return 0
    
    add_returncode(testout,result)
    
    if(check_file(testout,stdout)==0):
        print("\033[92mAccept \033[0m"+input)
        return 1
    else:
        print("\033[91mWrong Answer on \033[0m"+input)
        return 0
    
    

parser = argparse.ArgumentParser()
parser.add_argument('input_folder', default='testcase/functional_test')
parser.add_argument('output_folder', default='test_output/functional_testIR')
parser.add_argument('opt_level', type=int, default=0)
parser.add_argument('step', default="llvm")
args = parser.parse_args()

input_folder = args.input_folder
output_folder = args.output_folder
step = args.step
opt_arg = ""
if(args.opt_level == 0):
    opt_arg = "-O0"
else:
    opt_arg = "-O1"
if(step == "llvm"):
    ac=0
    total=0
    for file in os.listdir(input_folder):
        if(file.endswith(".sy")):
            total += 1
            name = file.split(".")[0]
            input = name+".in"
            output = name+".out"
            if(os.path.exists(input_folder + '/' + name + ".in")):
                ac += execute_ir(input_folder+'/'+file, output_folder+'/'+name+".ll", opt_arg, input_folder+'/'+input, input_folder+'/'+output, "tmp.out")
            else:
                ac += execute_ir(input_folder+'/'+file, output_folder+'/'+name+".ll", opt_arg, "none", input_folder+'/'+output, "tmp.out")
    print("IRTest-Grade:"+str(ac)+"/"+str(total))
    
if(step == "S"):
    ac=0
    total=0
    for file in os.listdir(input_folder):
        if(file.endswith(".sy")):
            total += 1
            name = file.split(".")[0]
            input = name+".in"
            output = name+".out"
            if(os.path.exists(input_folder + '/' + name + ".in")):
                ac += execute_asm(input_folder+'/'+file, output_folder+'/'+name+".s", opt_arg, input_folder+'/'+input, input_folder+'/'+output, "tmp.out")
            else:
                ac += execute_asm(input_folder+'/'+file, output_folder+'/'+name+".s", opt_arg, "none", input_folder+'/'+output, "tmp.out")
    print("ASMTest-Grade:"+str(ac)+"/"+str(total))
    
os.system("rm tmp.out")
os.system("rm a.out")
