# PopupUI
控制对话框显示类

## DialogOverlay
将QWidget显示为对话框，并添加阴影层。

1. 在CMakeLists.txt中导入库
```cmake
find_package(PopupUI 1.1.0 REQUIRED COMPONENTS DialogOverlay)

target_link_libraries(${PROJECT_NAME}
    PopupUI::DialogOverlay
)
```

2. 初始化
将需要在其之上显示对话框的控件设置为父窗口，通常是App主窗口。
```c++
#include <popupui/dialogoverlay/dialogoverlay.h>

using namespace PopupUI;

MyApp::MyApp(QWidget* parent)
  : QWidget(parent)
{
    //将MyApp注册为对话框显示的主窗口，并配置基础阴影层的颜色
    DialogOverlay::registerHostWindow(this, QColor(0, 0, 0, 128));
    //设置动画显示的默认值
    DialogOverlay::setFadeAnimationDefault(180);
}
```

3. 显示对话框
使用DialogOverlay显示对话框，非阻塞方式显示：
```c++
auto dlg = new MyDialog;
DialogOverlay::showDialog(dlg);
```
阻塞方式显示：
```c++
MyDialog dlg;
DialogOverlay::showDialogExec(dlg);
```

多个宿主窗口可注册不同 id。未指定宿主 id 时，仍使用第一个注册的宿主窗口：
```c++
DialogOverlay::registerHostWindow(this);
DialogOverlay::registerHostWindow(secondaryWindow, QStringLiteral("secondary"));

auto dlg = new MyDialog;
DialogOverlay::showDialog(dlg, PopupAnimationType::Fade,
    PopupProperty(QStringLiteral("secondary")));
DialogOverlay::closeTopDialog(QStringLiteral("secondary"));
```

宿主窗口销毁后，可在显示前检查其是否仍可用：
```c++
const auto hostId = QStringLiteral("secondary");
if (DialogOverlay::isHostWindowAvailable(hostId)) {
    auto dlg = new MyDialog;
    DialogOverlay::showDialog(dlg, PopupAnimationType::Fade, PopupProperty(hostId));
}
```

4. 显示过渡动画
```c++
auto dlg = new MyDialog;
//淡入淡出
DialogOverlay::showDialog(dlg, new FadePopupAnimation(180));
//缩放
DialogOverlay::showDialog(dlg, new ScalePopupAnimation(0.8, 180));
//平移
DialogOverlay::showDialog(dlg, new SlidePopupAnimation(SlideDirection::FromRight, 180));
//组合
DialogOverlay::showDialog(dlg, new ParallelPopupAnimation({ new FadePopupAnimation(180), new ScalePopupAnimation(0.8, 180) }));
```
可配置默认值，然后使用枚举：
```c++
//初始化时配置默认值
DialogOverlay::setFadeAnimationDefault(180);
DialogOverlay::setScaleAnimationDefault(180, 0.8);
DialogOverlay::setSlideAnimationDefault(180, SlideDirection::FromRight);

//显示对话框
auto dlg = new MyDialog;
//淡入淡出
DialogOverlay::showDialog(dlg, PopupAnimationType::Fade);
//缩放
DialogOverlay::showDialog(dlg, PopupAnimationType::Slide);
//平移
DialogOverlay::showDialog(dlg, PopupAnimationType::Scale);
//组合
DialogOverlay::showDialog(dlg, PopupAnimationType::Fade | PopupAnimationType::Slide);
```

5. 显示控制属性
通过属性控制显示细节：
```c++
auto dlg = new MyDialog;
DialogOverlay::showDialog(dlg, PopupAnimationType::Fade,
    PopupProperty(QStringLiteral("secondary")) //指定宿主窗口，不指定时使用第一个注册的宿主窗口
      .closeOnClickOutside(true) //是否点击阴影层关闭对话框
      .setMaskColor(QColor(255, 170, 255, 50)) //多个对话框同时显示时，设置当前对话框层的阴影，不影响其他层
      .transparentMouseEvent(true) //阴影层是否鼠标穿透
      .setDeleteOnClose(true) //是否关闭时删除对象
      .setDraggableArea(dlg->getDraggableTitleWidget(), true) //设置可拖拽的控件，并控制在应用窗口范围内拖动
      .showBaseMaskLayer(true) //是否显示基础阴影层
);
```
