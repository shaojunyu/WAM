# **基于WAM真核生物剪接位点预测**


---

> ##目录
1. 写在前面
2. 获取代码
3. 关于WAM
4. 代码说明
5. 尾巴

##   1.写在前面
关于WAM模型，我本来想写三个版本的代码,c、php、java，最先拿php实现了模型的功能，但是代码写的不够漂亮，在这里还是没有提交到库上。后来花了差不多五天实现用c重写，实现了全部功能，现托管于github上，与大家交流共享交流，欢迎大家批评指正！

### 1.1 关于开发环境的一些约定（以我的开发环境为例，类似的环境也OK）

 * 操作系统： Ubuntu Server 14.04 (Why linux，why not Windows? 我曾经也一直在问这个问题，现在我放弃纠结了...)
 * 编译器：gcc（gcc编译器默认不在ubuntu的发行包中，需要手动安装，执行这个命令即可：
 ` $ sudo apt-get install gcc `）
 * make编译（可选）
 * 如何获取代码：
 　　如果你了解git版本管理工具，可以在linux中直接使用git命令行获取代码，执行
 `$ git clone https://github.com/shaojunyu/WAM.git` 即可。（git软件包默认页不在ubuntu的发行包中，需要手动安装 `$ sudo apt-get install git`）
　　如果不喜欢git，也可以直接下载。点击`Download ZIP`就可以了![][2]，会直接下载一个压缩包，解压之后就可以看到文件了，代码位于`code`文件夹中.
[2]:https://raw.githubusercontent.com/shaojunyu/WAM/c/image/download.png
 * 文件目录结构,如下图所示。在code文件件中，有7个代码文件，一个makefile编译规则文件，外加两个数据文件夹`Testing` 和 `Training`,分别用于存储测试数据文件和训练数据文件，可以修改代码，但请勿随意修改目录结构！![][1]
  [1]: https://raw.githubusercontent.com/shaojunyu/WAM/c/image/dir.png

### 1.2 如何运行代码

 1. 编译 
编译可以使用这两个编译命令（二选一）
`$ gcc -o WAM_Bin main.c jwHash.c -lm `
`$ make `(如果你的linux环境中有make编译器)
 2. 运行
 输入命令`$ ./WAM_Bin` 即可运行
