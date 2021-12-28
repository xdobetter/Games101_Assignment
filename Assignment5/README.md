

# 思考

## 射线的生成

参考了[Ray-Tracing: Generating Camera Rays](https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays)
这个链接yyds，弄明白了射线是怎么生成的，很爽！！！

简单理一下变换的思路：
初始时为raster space，该space下就是初始的i,j，之后将其转到NDC(Normalize Device Coordinates) 空间，NDC space的特征是归一化了
其次从NDC space转到Screen space，Screen space是将坐标系变为了以屏幕中心的坐标系
最后从Screen space 转到 Camera space，在上述坐标系下进行Fov和Ratio的变换





## 三角形求交

![重心坐标系](./img/2.png)

games101的推导

该推导是怎么来的？

推导见[
Ray Tracing: Rendering a Triangle](https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection)

# Hint

1.隐式方程和显式方程的区别？

隐函数不一定能写为y=f(x)的形式，如x^2+y^2=0。显函数是用y=f(x)表示的函数，左边是一个y右边是x的表达式 比如y=2x+1。隐函数是x和y都混在一起的，比如2x-y+1=0。有些隐函数可以表示成显函数，叫做隐函数显化，但也有些隐函数是不能显化的，比如e^y+xy=1。


# Result

正确结果

<table rules="none" align="center">
	<tr>
		<td>
			<center>
				<img src="./img/result_2.png" width="900" />
				<br/>
				<font color="green">result_1</font>
			</center>
		</td>
	</tr>
	<tr>
		<td>
			<center>
				<img src="./img/result_3.png" width="900" />
				<br/>
				<font color="pink">result_2</font>
			</center>
		</td>
	</tr>
</table>





# Ref

[c++17 nodiscard attribute](https://blog.csdn.net/shongbee2/article/details/103942269)
[菲涅尔方程（Fresnel Equation）](https://zhuanlan.zhihu.com/p/375746359)
当入射光方向接近垂直表面时，大部分的能量会被折射，所以我们能看清水底的东西。而当入射光方向接近平行表面时，大部分的能量会被反射，所以我们会看见远处的倒影。
[折射定律](https://baike.baidu.com/item/%E6%8A%98%E5%B0%84%E5%AE%9A%E5%BE%8B/906357?fr=aladdin)
[PPM文件格式详解](https://blog.csdn.net/kinghzkingkkk/article/details/70226214)

[作业5Primary Ray的理解和计算问题](http://games-cn.org/forums/topic/zuoye5primary-raydelijiehejisuanwenti/)
[作业5：i、j和x、y存在着怎样的关系？](http://games-cn.org/forums/topic/zuoye5ijhexycunzaizhezenyangdeguanxi/)
[Ray-Tracing: Generating Camera Rays](https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays)
[【PBR系列四】PBR核心原理及材质属性查询](https://blog.csdn.net/qq_35312463/article/details/108111816?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2.essearch_pc_relevant&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2.essearch_pc_relevant)
> 参考这个链接，理解了折射率(IOR)的值对渲染效果的影响

[Whitted光线追踪实现](https://blog.csdn.net/qq_39300235/article/details/105520960)
>看到这个blog，我发现，这个A5作业的代码和这个也实在太像了吧，代码架构和算法简直一模一样。不过通过这个blog，我学会了反射项怎么算，
目前代码中还有3个地方还没有彻底弄懂，分别是
* [ ] 折射的推导
* [ ] 菲涅尔项的推导
* [ ] bias的推导