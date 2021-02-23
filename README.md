# AWPSOFT-GAME-MODULE #  
AWPSOFT 版权所有 ©2020-2021  

基于Direct2D、XAudio2等，使用Visual Studio C++构建简易2D游戏，支持编码为Unicode UTF-16LE的程序。  

需要C++17标准。  

最低支持的Windows系统为 **Windows7 SP1** ，最低支持的DirectX版本为 **DirectX11 June 2010** 。  
*仅支持在Windows桌面应用中使用。*  

# 如何使用 #
将**头文件所在目录**添加到include中，并在代码中使用：
```
#include <AwpsoftGameModule.h>
```   
本模组所有功能均在```namespace AwpSoftGameModule```中，且```AwpsoftGameModule.h```中存在```using namespace AwpSoftGameModule;```
  
如果不想```using namespace AwpSoftGameModule;```则需要：
```
#define _NOT_USING_NAMESPACE_AWPSOFT_GAME_MODULE  
#include <AwpsoftGameModule.h>  
```

如果编译为lib文件则需要：
```
#pragma comment(lib,"AwpsoftGameModule.lib")
```

**必须编译本模组，否则无法链接。**

# 特别注意 #
**本模组的所有public class均不是COM组件。但是函数可能返回COM接口。**

**任何线程在使用本模组前必须调用 ```CoInitializeEx(NULL, COINIT_MULTITHREADED);```**  
**使用本模组过程中不能调用CoUninitialize();**  
**为方便使用，本模组内部多次调用了 ```CoInitializeEx(NULL, COINIT_MULTITHREADED);```如果当前线程不需要再使用本模组和其他COM组件，你可以使用下列代码，强制卸载当前线程的所有COM组件：**
```
while(true)
{
	if(CoInitialize(NULL) == S_OK)
	{
		CoUninitialize();
		break;
	}
	CoUninitialize();
}
```


**如果当前线程调用了```CoInitialize(NULL);```,或者```CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)```等单线程COM组件初始化函数，则不能使用本模组的核心功能，除非先放弃先前使用的COM组件，并使用下列代码：**
```
while(true)
{
	if(CoInitialize(NULL) == S_OK)
	{
		CoUninitialize();
		break;
	}
	CoUninitialize();
}
```

**如果设置了显示缩放，即显示比例不是100%，可能会造成画面模糊，解决方法在后续文档中提供。**


# AsgmMath.h #

**extern const float PentagramX[6];**
**extern const float PentagramY[6];**
外接圆半径为五角星的中心和五个点坐标。
```
	const float PentagramX[6] = { 0.0f,0.0f,0.9510565f,0.5877852f, -0.5877852f, -0.9510565f }; 
	const float PentagramY[6] = { 0.0f,-1.0f,-0.3090167f,0.8090170f,0.8090170f,-0.3090167f };
/*
    1
4   0   3  
  2   5
*/
```


**float AsgmCosD(float thetaDEG);**  
**float AsgmSinD(float thetaDEG);**  
快速计算三角函数cos(x * 180/pi)，sin(x * 180/pi)，误差不超过0.0002。 

**float AsgmArctanD(float x);**  
快速计算反三角函数arctan(x) (-1 <= x <= 1)，结果为角度，误差不超过0.08°。  

**float AsgmSqrt1AddX(float x);**  
快速计算sqrt(1+x) (0 <= x <= 1)，误差不超过0.004。。  

**float AsgmDirectionD(float deltaX, float deltaY);**  
快速计算二维矢量的极角（方向），结果为角度。误差同AsgmArctanD.

**float AsgmDistance(float deltaX, float deltaY);**  
**float AsgmDistance(float deltaX, float deltaY, float deltaZ);**  
快速计算矢量的欧几里得范数（长度），对长度为1的二维矢量，误差不超过0.004。  

**float AsgmDistanceSquare(float deltaX, float deltaY);**
**float AsgmDistanceSquare(float deltaX, float deltaY, float deltaZ);**  
计算矢量的欧几里得范数的平方（长度的平方）。  

# CallbackTree.h #

**void EnterStdcallCallbackTree(StdcallCallbackTreeFunctionAddress root, void*  sharedParamsAddress);**  
*回调函数类型为 ```void*  __cdecl function(void *);```*   
**void EnterCdeclCallbackTree(CdeclCallbackTreeFunctionAddress root, void*  sharedParamsAddress);**  
*回调函数类型为 ```void*  __stdcall function(void  *);```*   
注意：调用约定和函数类型必须匹配！
CallbackTree流程控制函数，对不同的调用约定应分别使用对应的函数，例如：
```
#include <iostream>
#include <cstdio>
#using namespace std;
#char PARAM[80] = "锟斤拷烫烫烫";
void* __cdecl MsnekoSaid(void* param)
{
	cout<<"msneko：实用主义者都是无头苍蝇。"<<endl;
	printf("%s\n",(char*)param);
	return nullptr; //nullptr表示调用完即结束
}
void* __cdecl AwpsoftSaid(void*)
{
	cout<<"awpsoft：学院派都是瓜。"<<endl;
	return MsnekoSaid; //接下来调用MsnekoSaid
}
int main()
{
	EnterCdeclCallbackTree(AwpsoftSaid, &PARAM[0]);
	return 0;
}
```
运行结果为
```
awpsoft：学院派都是瓜。
msneko：实用主义者都是无头苍蝇。
锟斤拷烫烫烫

```

文档先写到这，以后想写了再写


