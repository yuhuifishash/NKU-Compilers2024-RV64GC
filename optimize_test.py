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
        result = execute_with_stdin_out("timeout 1 ./a.out > "+testout + " 2>/dev/null")
    else:
        result = execute_with_stdin_out("timeout 1 ./a.out < "+stdin+" > "+testout + " 2>/dev/null")
    if(result == 124):
        print("\033[93mTime Limit Execced on \033[0m"+input)
        return 0
    elif(result == 139):
        print("\033[95mRunTime Error on \033[0m"+input)
        return 0
    
    add_returncode(testout,result)
    
    if(check_file(testout,stdout)==0):
        print("\033[92mCorrect Answer on \033[0m"+input)
        return 1
    else:
        print("\033[91mWrong Answer on \033[0m"+input)
        return 0

input_folders = ["basic_mem2reg","eliUnreachablebb"]
output_folder = "test_output/functional_testIR"
opt_arg = "-O1"

for input_name in input_folders: 
    input_folder = "testcase/optimize_test/" + input_name   
    for file in os.listdir(input_folder):
        if(file.endswith(".sy")):
            name = file.split(".")[0]
            input = name+".in"
            output = name+".out"
            if(os.path.exists(input_folder + '/' + name + ".in")):
                execute_ir(input_folder+'/'+file, output_folder+'/'+name+".ll", opt_arg, input_folder+'/'+input, input_folder+'/'+output, "tmp.out")
            else:
                execute_ir(input_folder+'/'+file, output_folder+'/'+name+".ll", opt_arg, "none", input_folder+'/'+output, "tmp.out")
            if(input_name=="eliUnreachablebb"):
                result_ll = name + ".ll"
                stats = os.stat(output_folder+"/"+result_ll)
                if(stats.st_size > 8000):
                    print("\033[91mOptimize Test Failed \033[0m"+file)    
                else:
                    print("\033[92mOptimize Test Passed on \033[0m"+file)   
            if(input_name=="basic_mem2reg"):
                result_ll = name + ".ll"
                is_alloca=0
                with open(output_folder+"/"+result_ll) as res:
                    resfile = res.readlines()
                for line in resfile:
                    if "alloca" in line:
                       print("\033[91mOptimize Test Failed on \033[0m"+file)
                       is_alloca=1
                       break  
                if(is_alloca==0):
                    print("\033[92mOptimize Test Passed on \033[0m"+file)  
    
os.system("rm tmp.out")
os.system("rm a.out")