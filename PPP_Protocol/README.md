# 难处理的点
## 编码
+ __<font size="4"> 特殊字符位置记录 </font>__

+ __<font size="4"> 特殊字符有 0，帧头，帧尾，是必须 </font>__

+ __<font size="4"> 将记录位置0变为200，例如101 1 0此时100的位置，在buffer分离时以0为帧尾 </font>__

+ __<font size="4"> 常规字符跟后 </font>__

## 解码
+ __<font size="4"> 特殊字符位置记录解码 </font>__

+ __<font size="4"> 在常规字符写入时，会把写入0的位置，也给覆盖，所以，0得特殊处理，记下0的位置，写入时避开0的位置</font>__