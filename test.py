import subprocess
import os
import csv

# 定义要执行的可执行文件及其参数, 测试路径
paths = ['./tests/sytest/func'] 
executable_file = "./build/parser"
target_end = '.sy'
output_file_csv = './output.csv'

def test_one(arguments):

    keys = ['test_example','stderr','stdout','retcode','others']
    res = dict.fromkeys(keys)
    res['test_example'] = arguments
    res['others'] = []

    try:
        # 创建子进程并执行可执行文件
        process = subprocess.Popen([executable_file] + arguments, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # 获取标准输出和标准错误
        stdout, stderr = process.communicate()

        # 输出标准输出和标准错误内容
        res['stderr'] = stderr.decode("utf8")
        res['stdout'] = stdout.decode('utf8')

        # 获取返回码, 0表示成功
        return_code = process.returncode
        res['retcode'] = return_code

    except FileNotFoundError:
        res['others'].append('找不到可执行文件')
    except Exception as e:
        res['others'].append('其他异常')

    return res

def format_out(out_file,example_output):

    with open(out_file,'w',newline='') as file:
        writer = csv.writer(file)
        writer.writerow(example_output.keys())
        for item in zip(*example_output.values()):
            writer.writerow(item)

if __name__ == '__main__':
    
    folder_output = {
        'test_example' : [],
        'stderr': [],
        'stdout': [],
        'retcode': [],
        'others' : []
    }
    # test and write in folder_output
    for folder in paths:
        files = os.listdir(folder)
        for file in files:
            if file.endswith(target_end):
                ans_example = test_one(['/'.join([folder,file])])
                for key, val in ans_example.items():
                    folder_output[key].append(val)

    format_out(output_file_csv,folder_output)

                


