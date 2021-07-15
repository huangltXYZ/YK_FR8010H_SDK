2021.06.28
1. 支持应用层创建自己的可连接广播，从外部看可以看成两个独立的设备
2. 应用层还可以调用app_mesh_send_user_adv_packet发送自定义广播包，实现类似私有mesh中的数据通信。
3. 修改了proxy广播的机制
4. 加入了底层调度自检的功能，如果底层调度出现异常，会向应用层发出MESH_EVT_FATAL_ERROR消息，这时应用层需要重新启动设备。

2021.04.25
1. 支持握手打印时的freqchip字样更换为top-chip（需要在user_custom_parameter函数中调用retry_handshake，参数为0xFE）。
2. 增加配网失败的timer，获取更高的配网成功率。

2021.04.06
1. 优化了配网的稳定性

2021.03.26
1. 优化了配网的稳定性
2. 增加net layer的cache数量
3. 加入了用户自定义配置mesh PB-GATT和proxy中scan response内容的接口

2021.03.22
1. 支持GCC编译环境
2. 修改了mesh中的buffer分配机制，提升稳定性
3. 测试了三台设备同时入网、控制、删除，共20轮

2021.03.05
1. 程序会打印出来DEV_KEY、NETWORK_KEY、APP_KEY
2. 在proj_main文件中的user_entry_after_ble_init函数里面可以调用system_set_tx_power(RF_TX_POWER_NEG_16dBm);将发射功率配置到最低，同时断开板载天线，用来降低provisonee的信号强度，从而实现近距离测试PB-Remote功能。
3. 当provisonee作为从机被provioner连接上之后会打印
    slave[0],connect. link_num:1
    0x81,0x5D,0x54,0x42,0x5F,0x62,
	通过打印的地址来判断主机是echo还是其他节点，从而区分入网是否通过PB-Remote。