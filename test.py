import subprocess
import os
import csv
import json

# 定义要执行的可执行文件及其参数, 测试路径
# paths = ['./tests/sytest/func'] 
paths = ['./test/functional','./test/hidden_functional']
# executable_file = "./build/parser"
executable_file = "./build/cminusfc"
# executable_file = "./build/lexer"
target_end = '.sy'
outputasm_file_csv = './outputasm.csv'


#汇编文件目录
paths_of_asm = ['./test/functionalout','./test/hidden_functionalout']
cross_compiler_path = './toolchain-loongarch64-linux-gnu-cross/bin/loongarch64-linux-gnu-gcc'
output_executable_dir = ['./test/functional_exe','./test/hidden_functional_exe']


#可执行文件目录
qemu_path = './qemu-x86_64-to-loongarch64'
executable_output = './outrun.json'


def generate_asm():
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
                ans_example = generateasm_one(['/'.join([folder,file])],['-mem2reg','-S'],0b1101)
                # ans_example = test_one(['/'.join([folder,file])])
                for key, val in ans_example.items():
                    folder_output[key].append(val)

    format_outasm(outputasm_file_csv,folder_output)

def generateasm_one(arguments, other_argument = None, mask = 0b1111):

    keys = ['test_example','stderr','stdout','retcode','others']
    res = dict.fromkeys(keys)
    res['test_example'] = arguments
    res['others'] = []

    try:
        # 创建子进程并执行可执行文件
        process = subprocess.Popen([executable_file] + arguments + other_argument, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

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
    

    keylst = ['stderr','stdout','retcode','others']

    for i in range(len(keylst)):
        if mask >> i & 1 == 0:
            res[ keylst[i] ] = 'masked'

    return res

def format_outasm(out_file,example_output):

    with open(out_file,'w',newline='') as file:
        writer = csv.writer(file)
        writer.writerow(example_output.keys())
        for item in zip(*example_output.values()):
            writer.writerow(item)

def generate_executable():
    
    lenth_of_dir = len(paths_of_asm)
    error_examples = []

    for i in range(lenth_of_dir):
        source_dir = paths_of_asm[i]
        output_dir = output_executable_dir[i]

        asms = os.listdir(source_dir)
        for asm in asms:
            print(f'{asm} is generating')
            prefix,suffix = asm.split('.')
            if asm.endswith(suffix):
                cmd = ' '.join([cross_compiler_path,'-static', '/'.join([source_dir,asm]), './src/io/io.c', '-o', '/'.join([output_dir,prefix])])
                try:
                    subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell = True)
                except Exception as e:
                    error_examples.append(asm)
    print('fails:')
    print(error_examples)
    print('fails end')
    print('Done')

def test_executable():
    len_wait_test = len(output_executable_dir)
    all_dir_out = []
    for i in range(len_wait_test):
        source_dir = output_executable_dir[i]
        input_dir = paths[i]

        exes = os.listdir(source_dir)
        syfile = os.listdir(input_dir)
        input_files = set()
        output_files = set()
        dir_out = []

        for file in syfile:
            if file.endswith('.out'):
                output_files.add(file)
            elif file.endswith('.in'):
                input_files.add(file)

        for executable in exes:
            executable_out = {
                'name': executable,
                'stdout': None,
                'stderr': None,
                'retcode': 0,
                'except': [],
                'cmpare': None
            }

            input_argument = executable + '.in'
            cmd = ' '.join( [qemu_path,'/'.join([source_dir,executable]) ,'/'.join([input_dir,executable + '.in']) if input_argument in input_files else ''])
            try:
                process = subprocess.run( ' '.join(['chmod +x ' + qemu_path, '&&', cmd]) , shell = True,capture_output = True,text = True)
                retcode = process.returncode

                executable_out['stderr'] = process.stderr
                executable_out['stdout'] = process.stdout
                executable_out['retcode'] = retcode

                
                out_get = process.stdout
                out_get += str(retcode) + '\n'

                out_expected = ''
                with open( '/'.join([input_dir,executable + '.out']) ) as file:
                    out_expected = file.read()
                
                executable_out['cmpare'] = (out_get == out_expected)

            except Exception as e:
                executable_out['except'].append(e)
            
            dir_out.append(executable_out)
        
        all_dir_out.append(dir_out)
    print(all_dir_out)
    
    json_data = json.dumps(all_dir_out)
    with open(executable_output, "w") as json_file:
        json_file.write(json_data)

    print('Done')


            

if __name__ == '__main__':
    # generate_executable();
    test_executable()


                


