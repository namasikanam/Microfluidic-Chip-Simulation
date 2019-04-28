# Microfluidic Chip Simulation

2018年夏季学期基础程序设计训练第一周大作业.

- [Microfluidic Chip Simulation](#microfluidic-chip-simulation)
    - [User Guide](#user-guide)
    - [Features](#features)
    - [Development](#development)
    - [License](#license)

## User Guide

基本功能
- Initialize: 绘制指定结构的初始芯片.
    - 点击工具栏中的`Initialize`或通过快捷键`Alt-I`
- Process: 计算芯片流速/流量/浓度.
  - 点击工具栏中的`Process`或通过快捷键`Alt-P`
  - 流速/流量/浓度均经过归一化(Normalize)处理.
  - 该功能可使用的条件:
    - Initialized
    - 恰有2个输入管道
    - 恰有3个输出管道
    - 所有输入, 输出管道相连
  - 显示流速/流量/浓度均经过线性归一化(Normalize)处理.
- Maintain: 根据指定流速, 流量, 浓度, 在给定芯片基础上自动微调得到拥有更接近的输出的芯片.
  - 点击工具栏中的`Maintain`或通过快捷键`Alt-M`
  - 微调的结果综合考虑了指定流速, 流量和浓度.
  - 该功能可使用的条件:
    - Processed


图形化芯片设计界面
- 鼠标判定范围以宽度为200的管道(标准管道)为准.
    - 以防管道太小无法点击
- 左键单击添加/删除管道
    - 若管道不存在, 添加一个管道.
        - 若还原之前在此处被删除的管道依然合法, 则还原.
        - 若还原之前在此处被删除的管道会导致不合法情况出现, 则被添加的管道拥有可以允许的最大宽度.
        - 保证每时每刻每个管道都能以至少20μm的宽度被添加.
- 右键单击修改管道宽度
    - 与相邻管道的最小间距是200μm
    - 管道最小宽度是20μm
    - 若相邻管道不存在, 假设相邻的管道依然拥有20μm的最小间距.
- 鼠标置于管道上时, 显示管道当前宽度.
- 一旦有修改操作发生, 会将所有管道的颜色置白, 表示进入修改状态.

## Features

响应式布局
- 弹窗不允许修改窗口长宽.
- 管道显示宽度可根据窗口长宽调整.
- 3%的空白边界预留.
- 依据芯片大小设置窗口最小长宽.

界面设计
- 风格
    - 黑暗主题: 感谢ColinDuquesnoy提供的[QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet)
    - 纯白图标: 白色是管子们的颜色!
- 快捷键体系
    - 所有快捷键(`Alt-?`)均以下划线提示
    - 可使用`Ctrl-Tab`/`Ctrl-Shitf-Tab`在不同界面间切换.
- 管道颜色
    - 未经Process计算时, 所有管道是白色
    - 在Process/Maintain后会对管道上色
        - 不与输入/输出管道联通的管道是白色
        - 根据流速/流量/浓度对管道涂色.
            - 通过HSL色彩空间中, 色相(Hue)在[0,0.5]线性区分.
            - 即流速/流量/浓度为0, 对应管道是红色(Red), 流速/流量/浓度为1, 对应管道是青色(Cyan).

## Development

工程设计: 基本参照MVC架构
- `MainWindow`: 主界面及中央控制器, 负责基本功能调度.
- `Chip`: 存储芯片信息, 负责芯片算法.
- `Widget`: 负责绘制芯片, 接受鼠标事件.

算法部分
- 流速/流量计算: 高斯消元
- 浓度计算: 拓扑排序
- 自动化设计: 爬山

## License

鸣谢ColinDuquesnoy提供的[QStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet).