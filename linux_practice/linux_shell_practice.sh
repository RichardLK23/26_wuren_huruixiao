# 内容：编写一个 Shell 脚本，执行后须按顺序完成以下 6 项任务
#!/bin/bash

#    1.在当前目录下创建一个名为 linux_practice的文件夹，其内部包含两个子目录：docs 和 backup。
mkdir -p linux_practice/docs linux_practice/backup

#    2在 docs 目录下创建三个文件：readme.txt、notes.log 和 temp.tmp。
touch linux_practice/docs/readme.txt
touch linux_practice/docs/notes.log
touch linux_practice/docs/temp.tmp

#    3.删除 temp.tmp 文件，将 notes.log 重命名为 daily_report.txt
rm -f linux_practice/docs/temp.tmp
mv linux_practice/docs/notes.log linux_practice/docs/daily_report.txt

#    4.向 daily_report.txt 写入第一行内容：“Project Status: Active”
#                  追加第二行内容，显示当前系统日期（使用 date 命令）
echo "Project Status: Active" > linux_practice/docs/daily_report.txt
echo "$(date)" >> linux_practice/docs/daily_report.txt

#    5.将 docs 目录下的所有 .txt 文件复制到 backup 目录下。
cp linux_practice/docs/*.txt linux_practice/backup/

#    6.将 backup 目录下所有文件的权限修改为 只读  -r--r--r—
chmod 444 linux_practice/backup/*

#修改完成后，脚本需自动输出如下信息： Archive Complete. File [文件名] is now read-only.
echo "Archive Complete."
for file in linux_practice/backup/*; do
    filename=$(basename "$file")
    echo "File $filename is now read-only."
done

