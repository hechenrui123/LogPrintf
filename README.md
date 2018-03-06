# LogPrintf
LogPrintf工具为用户提供了在本地计算机上监视远程设备调试信息的能力


## 1.	功能概述
LogPrintf工具为用户提供了在本地计算机上监视远程主机调试信息的能力。通常情况下，可以在本地计算机上使用telnet程序，用它连接到服务器；但是在卫星网络的条件下，TCP连接质量较低，telnet连接很容易断开，影响调试体验。当服务器程序启动之后，各个待调试的网络设备的调试信息都会被收集到服务器。客户端程序启动时自动地在服务器上尝试注册，注册成功后调试信息会通过UDP协议传回本地，就像直接在服务器观察打印信息一样。本工具使用者可以在本地程序中修改配置文件来改变端口参数。

## 2. 程序结构框图

![Framework](https://github.com/hechenrui123/LogPrintf/blob/master/pic/Frame.png)

### 2.1 待调试设备上嵌入LogPrintf()函数
待调试设备每次调用LogPrintf()时，都会把打印信息通过UDP协议向服务器端发送。而且可以设置调试信息的type和level

### 2.2 服务器端程序分为device&client监听模块、发送模块：
Source监听模块监听在固定的端口13013，监听两类信息，其中A来自device，B来自Client：
#### * 等待调试设备发送的调试信息
这些信息会被打上type和level标签。根据标签信息，以及Client注册情况，服务器会调用发送模块，选择性转发调试信息给各个客户端。

#### * 接收来自客户端的控制信息
包括注册、查询、删除操作
 1.	客户端注册时，会带有两个参数numObj、ObjList，分别是客户端监听对象的个数和客户端监听对象。其中客户端监听对象是一个三元结构：<监听type，监听level，监听IP>。控制模块接受了这次注册后，会维护一个Client信息列表，作为服务器过滤、转发的依据。这个列表中每个客户端也包含一个定时器，定时器结束时则从列表中删除该客户端。最后调用发送模块返回注册操作结果。
 2.	客户端查询时，监听模块从客户端信息列表中查询，并返回结果。
 3.	客户端删除时，监听模块更新客户端信息列表，并返回结果
 
 发送模块负责向客户端发送控制信息和普通调试信息，通过字段的不同取值来区分控制信息和普通调试信息。服务器端在内存维护一个客户端信息列表，每项包括1.监听主体、2.定时器、3.监听目标列表（包括目标IP、目标type、目标level）。监听目标列表是过滤普通调试信息的依据。
 
### 2.3 客户端程序分为监听模块、server请求模块、本地输出模块
监听模块监听用户的键盘输入，以及服务器端的控制信息输入：

 1.	程序启动或者用户输入注册命令时，监听模块尝试出一个可行的端口，并且通过server请求模块向服务器端申请，成功后可以得到返回结果，在本地打印结果。如果注册成功，则开始监听申请成功的端口，并创建一个定时器，定时器结束后再次向服务器注册。
 2.	用户输入查询命令时，通过server请求模块向服务器查询。
 3.	用户输入关闭命令时，通过server请求模块向服务器申请注销。 
 4.	当监听的socket收到来自server的控制信息或者普通调试信息时，在本地打印。

Server请求模块根据不同的请求，向server发送不同格式的信令。
本地打印模块负责把收到的控制信息和普通调试信息打印。

## 3.	主要过程流程图
### 3.1	初始化过程
![init](https://github.com/hechenrui123/LogPrintf/blob/master/pic/Init.png)

初始化过程中，客户端首先尝试一个可行的端口，然后向服务器端申请，服务器端更新后，设置定时器，向客户端返回本次初始化的结果。客户端收到结果后，也启动一个定时器。服务器端在内存维护一个客户端信息列表，每项包括1.监听主体、2.定时器、3.监听目标列表（包括目标IP、目标type、目标level）

### 3.2	客户端关闭过程
![init](https://github.com/hechenrui123/LogPrintf/blob/master/pic/Close.png)

客户端关闭时，先通过端口通知服务器端注销这个地址，服务器端注销成功后，会更新地址列表，并返回操作结果。最后客户端再关闭socket，完成整个关闭过程。

## 4.	外部通信接口
### 4.1	Devices => Server
MSG|TYPE|LEVEL|STRING
:-----:|:-----:|:-----:|:-----:
1B|1B|1B|LENGTH

MSG=0代表这段消息是普通的调试信息，待调试设备向服务器发送打标后的调试信息，标志包括类型TYPE和级别LEVEL。

### 4.2	Client => Server
MSG=1代表这段消息是控制信息，控制信息又分为以下三类：

#### 4.2.1	申请（OP=0x00）
 
MSG |	OP |	PORT |	NUM |	TYPE0 |	LEVEL0 |	IP0 |	TYPE1 |	LEVEL1 |	IP1 |	……
:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:
1B |	1B |	2B |	2B |	1B |	1B |	4B |	1B |	1B |	4B |	……

申请时OP=0x00，num表示客户端的监听目标的个数，每个监听目标都包含三项：监听类型、监听级别、监听IP地址。其中监听类型以独热方式编码，可以通过掩码选择多个类型；其中的监听级别，表示需要监听的最低级别。

#### 4.2.2	查询（OP=0x10）和删除（OP=0x20）

MSG |	OP
:----:|:----:
1B |	1B

### 4.3	Server => Client
#### 4.3.1	普通调试信息
 
 MSG |	TYPE |	LEVEL |	srcIP |	STRING
 :----:|:----:|:----:|:----:|:----:|
 1B |	1B |	1B |	4B |	LENGTH | B
 
 MSG字段表示这段信息是普通调试信息还是控制信息。STRING字段代表的调试信息，经过服务器的处理已经包含TYPE、LEVEL和IP字段，是完整的调试信息，客户端接受后，可以直接在本地打印。

#### 4.3.2	控制信息、申请(删除)返回结果
 
MSG |	OP |	RES
:----:|:----:|:----:
1B	| 1B |	1B

#### 4.3.3 	控制信息、查询返回结果

MSG |	OP |	PORT |	NUM |	TYPE0 |	LEVEL0 |	IP0 |	TYPE1 |	LEVEL1 |	IP1 |	……
:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:
1B |	1B |	2B |	2B |	1B |	1B |	4B |	1B |	1B |	4B |	……

### 4.4 各字段具体取值
 
OP值 |	操作
:----:|:----:
0x00 |	注册
0x10 |	查询
0x20 |	删除

MSG值 |	类别
:----:|:----:
0 |	普通调试信息
1 |	控制信息

RES值 |	结果
:----:|:----:
0 |	成功
1 |	失败

LEVEL字段仅仅使用高四位，为保留后四位可以用于更精细的信息分级。

LEVEL值 |	信息级别
:----:|:----:
0x00 |	Verbose（详细）
0x10 |	Debug（调试）
0x20 |	Info（通告）
0x30 |	Warning（警告）
0x40 |	Error（错误）

TYPE值设计为每个bit位代表一种类型是因为和级别LEVEL不同，用户选择一个LEVEL就是选择了这个LEVEL之上的所有LEVEL。因为类型之间没有偏序关系，所以这样设计可以使用掩码来选择任意个数的TYPE类型。

TYPE值 |	信息类型
:----:|:----:
0x01 |	类型0
0x02 |	类型1
0x04 |	类型2
0x08 |	类型3
0x10 |	类型4
…… |	……

## 5.	配置文件格式
 
 1）	服务器配置文件记录：监听端口、客户端信息保存时间
 2）	客户端配置文件记录：服务器IP和监听端口、客户端尝试申请的起始端口、重新注册时间、客户端监听的目标列表
 3）	待调试设备配置文件：服务器IP和服务器监听端口
 
Server.ini
|:-|
[Server]
;服务器启动时监听的两个端口
Port=13013 
LiveTime=600

Client.ini
|:-|
[Server]
;客户端初始化时连接服务器的地址
ServerPort=13013 
IP="192.168.1.12"
[Client]
;客户端尝试申请的起始端口号
Port=14932
LiveTime=500
[Obj0]
;客户端监听的目标IP
IP="192.168.3.112"
;客户端监听类型0和类型2
TYPE=5
;客户端监听级别2以上的所有级别
LEVEL=2

Device.ini
|:-|
[Server]
;调用Logprint()时连接服务器的地址
ServerPort=13013 
IP="192.168.1.12"

## 6.	类的详细设计

### 6.1	客户端

![clientclass](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ClientClass.png)

LogClientAgent是客户端程序的代理类，Init()负责初始化，Run()中循环处理网络事件，LogClientAgent::ProcessMessage()处理网络事件的内容，根据内容的不同类型通过FunctionManger类来处理。TimerManager::ProcessTimer()负责处理网络事件的时间。客户端只有一个定时器，定时器到时后重新向服务器端注册，然后重启定时器。

### 6.2 服务器端

![serverclass](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ServerClass.png)

LogServerAgent是服务器端程序的代理类，Init()负责初始化，Run()中循环处理网络事件，LogClientAgent::ProcessMessage()处理网络事件的内容，根据内容的不同类型分别通过FunctionManger类、DispatchFilter类来处理。TimerManager::ProcessTimer()负责处理网络事件的时间。服务器为每一个注册的客户端初始化一个定时器，定时器到时后，删除对应的客户端的注册信息，然后关闭定时器。

### 6.3 设备端

![deviceclass](https://github.com/hechenrui123/LogPrintf/blob/master/pic/DeviceClass.png)

设备端的类LogPrintInterface功能是为用户提供函数LogPrintf()，调用时会向服务器输出调试信息。可以改变调试信息的TYPE和LEVEL。

## 7.	功能测试

![testframe](https://github.com/hechenrui123/LogPrintf/blob/master/pic/TestFrame.png)

**1.	Server的配置为：监听端口13013，连接保持时间20s。Server只在162.105.85.235上运行。** 
 
**2.	Interface(): 调试信息来源，模拟三个待调试设备device。发送五个不同级别的调试信息，每个级别各两条，来测试服务器的过滤功能。具体调试信息如下： **

![interfacetest](https://github.com/hechenrui123/LogPrintf/blob/master/pic/InterfaceTest.png)

**3.	Client0: 收听所有IP，所有TYPE、所有LEVEL的信息 **
 
**4.	Client1: 收听所有IP，TYPE=0x01或者TYPE=0x10、所有LEVEL的信息 **
 
**5.	Client2: 收听IP=162.105.85.69、所有TYPE、LEVEL>=0x30的信息 **

如下是各个Client的配置文件：

client0.ini
|:-|
[Server]
;初始化时连接服务器的地址
ServerPort=13013 
IP=162.105.85.235
[Client]
;客户端尝试申请的端口号
Port=14013
LiveTime=10000
objnum=3
[Obj0]
;客户端监听的目标IP
IP=162.105.85.69
;客户端监听所有类型
TYPE=255
;客户端监听所有级别
LEVEL=0
[Obj1]
IP=162.105.85.118
TYPE=255
LEVEL=0
[Obj2]
IP=162.105.85.235
TYPE=255
LEVEL=0	

client1.ini
|:-|
[Server]
;初始化时连接服务器的地址
ServerPort=13013 
IP=162.105.85.235
[Client]
;客户端尝试申请的端口号
Port=14013
LiveTime=10000
objnum=3
[Obj0]
;客户端监听的目标IP
IP=162.105.85.69
;客户端监听所有类型
TYPE=17
;客户端监听所有级别
LEVEL=0
[Obj1]
IP=162.105.85.118
TYPE=17
LEVEL=0
[Obj2]
IP=162.105.85.235
TYPE=17
LEVEL=0	

client2.ini
|:-|
[Server]
;客户端初始化时连接服务器的地址
ServerPort=13013 
IP=162.105.85.235
[Client]
;客户端尝试申请的起始端口号
Port=14013
LiveTime=10000
objnum=3
[Obj0]
;客户端监听的目标IP
IP=162.105.85.69
;客户端监听所有类型
TYPE=255
;客户端监听所有级别
LEVEL=48

**测试结果：**

1. Client0输出：

从输出中可以看出，client0不加过滤地接收了来自所有device的所有调试信息。

![clienttest00](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ClientTest00.png)
![clienttest01](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ClientTest01.png)

2.	Client1输出：

可以看出Client1接收了所有IP的、TYPE=0x01和0x10的调试信息。

![clienttest10](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ClientTest10.png)
![clienttest11](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ClientTest11.png)

3.	Client2输出：

Client2接收到了来自IP=162.105.85.69的、LEVEL=0x30、0x40、0x50的调试信息。

![clienttest2](https://github.com/hechenrui123/LogPrintf/blob/master/pic/ClientTest2.png)


