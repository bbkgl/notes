# Linux学习

## 文件和目录管理

### 权限

> sudo chmod 777 -R XXX(文件或目录)

- sudo chmod 600 ××× （只有所有者有读和写的权限）
- sudo chmod 644 ××× （所有者有读和写的权限，组用户只有读的权限）
- sudo chmod 700 ××× （只有所有者有读和写以及执行的权限）
- sudo chmod 666 ××× （每个人都有读和写的权限）
- sudo chmod 777 ××× （每个人都有读和写以及执行的权限）

ｘｘｘ可以是文件名也可以是单个文件，中间加的　`-R` 是递归这个目录下的所有目录和文件。



## Git

> 参考：[git - 简易指南](http://www.bootcss.com/p/git-guide/)

**创建新仓库**

1. 创建新文件夹

2. `git init`
3. 创建成功新的仓库

**检出仓库**

- 执行以下命令以创建一个本地仓库的克隆版本`git clone /path/to/respository`

- 如果是远程服务器上的仓库，你的命令会是这个样子：`git clone username@host:/path/to/repository`

**工作流**

你的本地仓库由git维护的三棵树组成。第一个是你的`工作目录`，它持有实际文件；第二个是`缓存区（Index）`，它像个缓存区域，临时保存你的改动；最后是`HEAD`，指向你最后一次提交后的结果。

![img](assets/trees.png)

**添加与提交**
1. 你可以计划改动（把它们添加到缓存区），使用如下命令：
	- `git add <filename>`
	- `git add *` 目录下所有命令？
2. 使用如下命令提交改动：
	- `git commit -m "提交信息"`
3. 你的改动已经提交到了`HEAD`，但是还没到你的远程仓库

**推送改动**
经过上述操作，你的改动现在已经在本地仓库的`HEAD`中了。执行如下命令以将这些改动提交到远程仓库：
- `git push origin master`（可以把*master*换成你想要推送的任何分支）。
如果现在还没有克隆现有仓库，并欲将你的仓库连接到某个远程服务器，你可以使用如下命令添加：
- `git remote add origin <server>`
这样就能把改动推送到服务器上去了

**分支**
分支是用来将特性开发绝缘开来的。在你创建仓库的时候，`master`是“默认的”。在其他分支上进行开发，完成后再将它们合并到主分支上。 

![img](assets/branches.png)

1. 创建一个叫“feature_x”的分支，并切换过去：
	- `git checkout -b feature_x`
2. 切换回主分支：
	- `git checkout master`
3. 再把新建的分支删除掉：
	- `git branch -d feature_x`
4. 除非你将分支推送到远程仓库，不然该分支就是*不为他人所见的*：
	- `git push origin <branch>`

**更新与合并**
1. 要更新你的本地仓库至最新改动，执行：
    - `git pull`
2. 以在你的工作目录中 *获取（fetch）* 并 *合并（merge）* 远端的改动。要合并其他分支到你的当前分支（例如 master），执行：
    - `git merge <branch>`
3. 两种情况下，git 都会尝试去自动合并改动。不幸的是，自动合并并非次次都能成功，并可能导致 *冲突（conflicts）*。这时候就需要你修改这些文件来人肉合并这些 *冲突（conflicts）* 了。改完之后，你需要执行如下命令以将它们标记为合并成功：
    - `git add <filename>`
4. 在合并改动之前，也可以使用如下的命令查看：
    - `git diff <source_branch> <target_branch>`

**替换本地改动**
- 加入你做错事，你可以使用如下命令替换改动
    - `git checkout --<filename>`
- 上述命令会使用 HEAD 中的最新内容替换掉你的工作目录中的文件。已添加到缓存区的改动，以及新文件，都不受影响。 
- 假如你想要丢弃本地的改动与提交，可以到服务器上获取最新的版本并将并将你本地分支指向它：
    - `git fetch origin`
    - `git reset --hard origin/master` 
