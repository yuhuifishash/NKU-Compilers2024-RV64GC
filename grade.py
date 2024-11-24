import argparse
import os


# 第一个参数表示测试哪个lab，第二个参数表示测试基本要求还是进阶要求
# python3 grade.py 3 1 (测试lab3进阶要求)
# python3 grade.py 5 0 (测试lab5基本要求)


parser = argparse.ArgumentParser()
parser.add_argument('lab', type=int, default=1)
parser.add_argument('is_advance', type=int, default=1)
args = parser.parse_args()
if(args.lab == 1 or args.lab == 2):
    print("lab1和lab2不支持自动评测, 请自行检查输入和输出")
elif(args.lab == 3):
    os.system("rm -rf test_output/functional_testIR/*.ll")
    if(args.is_advance==False):
        os.system("python3 test.py testcase/functional_test/Basic test_output/functional_testIR 0 llvm")
    else:
        os.system("python3 test.py testcase/functional_test/Advanced test_output/functional_testIR 0 llvm")
elif(args.lab == 4):
    os.system("rm -rf test_output/functional_testIR/*.ll")
    if(args.is_advance==False):
        os.system("python3 test.py testcase/functional_test/Basic test_output/functional_testIR 1 llvm")
    else:
        os.system("python3 test.py testcase/functional_test/Advanced test_output/functional_testIR 1 llvm")
elif(args.lab == 5):
    os.system("rm -rf test_output/functional_testAsm/*.s")
    if(args.is_advance==False):
        os.system("python3 test.py testcase/functional_test/Basic test_output/functional_testAsm 1 S")
    else:
        os.system("python3 test.py testcase/functional_test/Advanced test_output/functional_testAsm 1 S")
else:
    print("未知lab, 请检查输入")

    