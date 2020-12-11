# PathPlanningProject
В этом проекте исследуются и реализуются различные алгоритмы поиска оптимального пути. На данный момент реализован алгоритм A<sup>*</sup> и алгоритм Дейкстры.

## Инструкции запуска
Скрипты для запуска находятся в папке Scripts.
Находясь в папке Scripts, собрать и запустить Release-версию проекта на готовом примере, лежащем в Examples, можно так:
```bash
./BuildRelease
./RunRelease
```
А Debug-версию так:
```bash
./BuildDebug
./RunDebug
```
Запустить на другом тесте можно так:
```cmd
cd ../../Bin/{Debug|Release}/
PathPlanning.exe YourTest.xml
```
## Формат входного файла
Формат файла XML.
* Описание карты
  * width, height - размеры карты
  * cellsize - размер одной клетки на карте
  * startx, starty - координаты стартовой точки
  * finishx, finishy - координаты финишной точки
  * grid - карта в виде матрицы из 0 и 1, где 0 - свободная клетка, 1 - препятствие.   
* Описание типа поиска
  * searchtype - Применяемый алгоритм  
    * astar - Алгоритм А*
    * dijkstra - Алгоритм Дейкстры
  * metrictype - вид метрики
    * diagonal - диагональное расстояние  

    * manhatten - манхэттенское расстояние  
    
    * euclidian - евклидово расстояние  

    * chebyshev - расстояние Чебышева  
    
  * hweight - вес эвристики
  * allowdiagonal - если true, то разрешены переходы по диагоналям
  * cutcorners - если true, то разрешено срезать углы по диагонали
  * allowsqueeze - если true, то разрешено "протискиваться" через углы рядом стоящих препятствий
  
## Формат выходного файла
* numberofsteps - количество шагов алгоритма
* nodescreated - количество созданных объектов типа Node
* length - длина искомого пути в клетках сетки
* length_scaled - длина полученного пути
* time - время работы алгоритма
* path - представление итогового пути на карте
* lplevel - последовательность клеток, образующих путь
* hplevel - последовательность отрезков пути.
