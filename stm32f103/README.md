# 1. 查看文件改动状态
git status

# 2. 把所有修改加入暂存
git add .

# 3. 提交本地仓库（双引号里写改了什么）
git commit -m "修复bug/新增功能/优化页面"

# 4. 推送到 GitHub 远程
git push

# 5. 拉取 GitHub 最新代码
git pull

git init                          # 本地初始化git仓库
git remote add origin 仓库地址     # 关联远程GitHub仓库
git branch -M main                # 改主分支为main
git push -u origin main           # 首次推送关联分支

git branch                        # 查看所有分支
git checkout -b 分支名             # 创建并切换到新分支
git checkout 分支名               # 切换已有分支
git checkout main                 # 切回主分支
git merge 分支名                  # 把其他分支合并到当前分支
git branch -d 分支名              # 删除本地无用分支

# 撤销单个文件的修改（未提交前）
git checkout -- 文件名

# 撤销所有本地改动，恢复到上次提交（慎用）
git reset --hard HEAD

# 撤销最近一次提交，保留代码改动
git reset --soft HEAD~1

# 远程仓库已存在，更换地址
git remote remove origin

# 本地远程仓库冲突合并
git pull origin main --allow-unrelated-histories