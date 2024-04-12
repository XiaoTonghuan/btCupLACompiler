import subprocess
import os
import csv
import json

# 定义要执行的可执行文件及其参数, 测试路径
# paths = ['./tests/sytest/func'] 
# paths = ['./test/functional','./test/hidden_functional']
paths = ['./test/performance']
# executable_file = "./build/parser"
executable_file = "./build/cminusfc"
# executable_file = "./build/lexer"
target_end = '.sy'
outputasm_file_csv = './outputasm.csv'


#汇编文件目录
# paths_of_asm = ['./test/functionalout','./test/hidden_functionalout']
paths_of_asm = ['./test/performanceout']
cross_compiler_path = './toolchain-loongarch64-linux-gnu-cross/bin/loongarch64-linux-gnu-gcc'
# output_executable_dir = ['./test/functional_exe','./test/hidden_functional_exe']
output_executable_dir = ['./test/performance_exe']
asmout_output = './outasm.json'

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
        sorted(files)
        for file in files:
            if file.endswith(target_end):
                prefix,suffix = file.split('.')
                print(f'{file} start')
                ans_example = generateasm_one(['/'.join([folder,file])],['-mem2reg','-S', '-o', '/'.join([folder+'out',prefix+'.s']) ],0b1101)
                # ans_example = test_one(['/'.join([folder,file])])
                for key, val in ans_example.items():
                    folder_output[key].append(val)
                print(f'{file} end' )
    print('generate end')

    format_outasm(outputasm_file_csv,folder_output)

def generateasm_one(arguments, other_argument = None, mask = 0b1111):

    keys = ['test_example','stderr','stdout','retcode','others']
    res = dict.fromkeys(keys)
    res['test_example'] = arguments
    res['others'] = []

    try:
        # 创建子进程并执行可执行文件
        cmd = ' '.join([executable_file] + arguments + other_argument)
        process = subprocess.run(cmd, shell = True, timeout = 60)
        
        # 输出标准输出和标准错误内容
        res['stderr'] = process.stderr
        res['stdout'] = process.stdout

        # 获取返回码, 0表示成功
        return_code = process.returncode
        res['retcode'] = return_code

    except FileNotFoundError:
        res['others'].append('找不到可执行文件')
    except TimeoutError:
        res['others'].append('运行时间超时')
    except Exception as e:
        res['others'].append(e)
    

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

    all_asms = []
    for i in range(lenth_of_dir):
        source_dir = paths_of_asm[i]
        output_dir = output_executable_dir[i]

        asms = os.listdir(source_dir)
        one_asms = []

        for asm in asms:
            asm_out = {
                'name' : asm,
                'stdout': None,
                'stderr': None,
                'retcode': 0,
                'other': []
            }
            print(f'{asm} is generating')
            prefix,suffix = asm.split('.')
            if asm.endswith(suffix):
                cmd = ' '.join([cross_compiler_path,'-static', '/'.join([source_dir,asm]), './src/io/io.c', '-o', '/'.join([output_dir,prefix])])
                try:
                    process = subprocess.run(cmd, shell = True, timeout = 20, text = True, capture_output = True)
                    asm_out['stderr'] = process.stderr
                    asm_out['stdout'] = process.stdout
                    asm_out['retcode'] = process.returncode
                except Exception as e:
                    error_examples.append(asm)
                    asm_out['other'].append(e)
            one_asms.append(asm_out)
        all_asms.append(one_asms)

    json_data = json.dumps(all_asms)
    with open(asmout_output, "w") as json_file:
        json_file.write(json_data)
    
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
        sorted(exes)
        for executable in exes:

            print(f'{executable} start')

            executable_out = {
                'name': executable,
                'stdout': '',
                'stderr': '',
                'retcode': 0,
                'except': [],
                'cmpare': None
            }

            input_argument = executable + '.in'
            cmd = ' '.join( [qemu_path,'/'.join([source_dir,executable]) , ' < ' + '/'.join([input_dir,executable + '.in']) if input_argument in input_files else ''])
            try:
                process = subprocess.run( ' '.join(['chmod +x ' + qemu_path, '&&', cmd]) , shell = True,capture_output = True,text = True,timeout = 20)
                retcode = process.returncode

                executable_out['stderr'] = process.stderr
                # executable_out['stdout'] = process.stdout
                executable_out['retcode'] = retcode

                
                out_get = process.stdout

                out_expected = ''
                with open( '/'.join([input_dir,executable + '.out']) ) as file:
                    out_expected = file.read()
                
                executable_out['cmpare'] = (out_get + str(retcode) + '\n' == out_expected or out_get + '\n' + str(retcode) + '\n' == out_expected)

            except Exception as e:
                executable_out['except'].append('error')
            
            dir_out.append(executable_out)

            print(f'{executable} end')
        
        all_dir_out.append(dir_out)
    print(all_dir_out)
    
    json_data = json.dumps(all_dir_out)
    with open(executable_output, "w") as json_file:
        json_file.write(json_data)

    print('Done')


            

if __name__ == '__main__':
    generate_asm()
    generate_executable();
    test_executable()


                


