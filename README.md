#net-filter-extension

##src directory
- client    
-       客户端相关的逻辑实现
- common    
-       共有类型，组件的相关声明
- filter    
-       防火墙声明和实现
- net       
-       网络相关的声明和实现，包括协议，socket，http
- rule      
-       规则组件的逻辑实现
- server    
-       服务主进程的逻辑实现
- util      
-       底层工具的声明与实现
- monitor
-       相关监控的编写


## dev build
``` 
    docker build -t sjtu/yaolihong .
    ./run.sh
    make
```