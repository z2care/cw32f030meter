/*
key功能：1.功能切换 2.开关机（触发deepsleep和退出deepsleep） 3.
休眠功能，超过1分钟进deepsleep
串口输出数据折线功能，开发PC python界面
*/
/*
TEST.VO TEST.CU TEST.-O
CAL.I05 I10 U05 U10
t：
E：
S：
U：
O：
C：
-：
*/
K1短按切换模式

K2确认进入模式，以及显示和隐藏当前模式

K3深睡和唤醒（开关机）

LED测通断时亮，校准结束闪烁

BEEP通断时响，校准结束响，3分钟关机BEEP

t:defg:78
E:adefg:79
S=5:acdfg:6D
U:bcdef:3E
o:cdeg:5C
C:adef:39
u:cde:1C
t.:DPdefg:F8
-:g:40
A:abcefg:77
L:def:38
I:ef:30
0:abcdef:7F
