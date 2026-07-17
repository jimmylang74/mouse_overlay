# Mouse Overlay

Windows 全局鼠标钩子工具，在鼠标悬停的窗口周围绘制红色矩形边框，并在右键点击时输出窗口信息的 JSON。

## 功能

- 全局低级鼠标钩子 (`WH_MOUSE_LL`)，拦截所有鼠标事件
- 鼠标悬停时自动识别窗口并绘制红色矩形边框
- 右键点击输出 JSON 格式的窗口句柄、标题、矩形区域和鼠标坐标
- 支持 DPI 感知（Per-Monitor V2）

## 编译

使用 MinGW 交叉编译：

```bash
./build.sh
```

或手动编译：

```bash
x86_64-w64-mingw32-gcc \
    main.c \
    mouse_hook.c \
    overlay_window.c \
    window_finder.c \
    -lgdi32 \
    -luser32 \
    -o mouse_overlay.exe
```

## 使用

```bash
# 正常运行（静默模式，仅右键输出 JSON）
mouse_overlay.exe

# 调试模式（输出详细日志 + 右键 JSON）
mouse_overlay.exe --debug-verbose
```

## 命令行参数

| 参数 | 说明 |
|------|------|
| `--debug-verbose` | 启用调试输出，显示鼠标事件、窗口信息等详细日志 |

## 右键输出格式

右键点击时输出 JSON：

```json
{
  "handle": "0x000A0B2C",
  "title": "Notepad",
  "rect": {
    "left": 100,
    "top": 50,
    "right": 800,
    "bottom": 600
  },
  "mouse": {
    "x": 450,
    "y": 320
  }
}
```

## 文件结构

- `main.c` — 程序入口，命令行解析，初始化
- `mouse_hook.c/h` — 全局鼠标钩子安装与事件处理
- `overlay_window.c/h` — 全屏透明覆盖窗口，绘制红色矩形
- `window_finder.c/h` — 根据鼠标坐标查找目标窗口
- `globals.h` — 全局变量和数据结构定义
- `build.sh` — 编译脚本

## 依赖

- MinGW-w64 交叉编译工具链
- Windows API (`user32`, `gdi32`)
