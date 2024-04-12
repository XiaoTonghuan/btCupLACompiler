#!/bin/bash

# 指定你的文件夹路径
folder="./test/functional_exe"
input_folder="./test/functional"

# 检查文件夹是否存在
if [ ! -d "$folder" ]; then
    echo "文件夹不存在：$folder"
    exit 1
fi

if [ ! -d "$input_folder" ]; then
    echo "文件夹不存在: $input_folder"
    exit 1
fi

# 收集输出的文件名
output_file="time.txt"

# 删除旧的输出文件
rm -f "$output_file"

# 遍历文件夹中的所有可执行文件
for file in "$folder"/*; do
    found=0
    for file_input in "$input_folder"/*; do
        prefix="${file_input%%.in}"

         if [ -x "$file" ]; then
    
            if [ "$prefix"="$file" ]; then
                found=1
                echo "运行 $file"
                if [ $? = 0 ]; then
                    ./"$file" < "$file_input" 2>> "$output_file"  # 执行文件并将输出追加到输出文件中
                    echo "完成 $file"
                fi
            fi

        fi

    done

    if [ found=0 ]; then
        if [ -x "$file" ]; then
            if [ $? = 0 ]; then
                echo "运行 $file"
                ./"$file" 2>> "$output_file"  # 执行文件并将输出追加到输出文件中
                echo "完成 $file"
            fi
        fi
    fi

done

echo "所有可执行文件执行完成，并将输出保存到 $output_file"
