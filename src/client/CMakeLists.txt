#定义了一个SRC LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)

#指定生成可执行文件   可执行文件的名字叫Chatclient
add_executable(ChatClient ${SRC_LIST})


#指定可执行文件链接需要依赖的库文件
target_link_libraries(ChatClient pthread)