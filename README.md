

兵棋推演可视化。


- 使用多线程异步读取解析节点数据，解决数据解析性能卡点。
- 实现正交摄像机和透视摄像机，使用键鼠交互浏览地图。
- 对前后两秒数据按时间插值，实现高帧率平滑过渡。

##### main.cpp

基于opengl配置渲染流程，通过Data类获取点线数据，通过OrthoCamera类控制摄像机位置与键鼠交互。在渲染循环中，先基于图片在空间中绘制四边形地图，再根据点数据和连线数据绘制分别绘制点和线。

##### data_producer_multi.h

定义了点、线、点线结构体和数据解析类，数据解析类通过多线程解析不同json file的数据存储到dataMap中，dataMap满了就停止解析，当主渲染线程取走数据时，就继续解析数据。

##### ortho_camera.h

定义了正交摄像机，通过键鼠交互控制摄像机移动，WASD控制上下左右，鼠标滚轮控制放大缩小。

##### shader.h

定义了着色器类，实现着色器的编译与链接。

说明：基于opengl的渲染管线可查看[opengl-cn](https://learnopengl-cn.github.io/01%20Getting%20started/01%20OpenGL/) 入门部分的内容，渲染部分的代码与该网站提供的代码基本一致，该网站有对代码的详细说明和对渲染管线的详细介绍。项目的部署运行参考opengl-cn[创建窗口](https://learnopengl-cn.github.io/01%20Getting%20started/02%20Creating%20a%20window/)部分，结合上述教程和项目代码的注释很简单易懂。





