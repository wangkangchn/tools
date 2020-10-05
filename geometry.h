/***************************************************************
Copyright © wkangk <wangkangchn@163.com>
文件名		: geometry.h
作者	  	: wkangk <wangkangchn@163.com>
版本	   	: v1.0
描述	   	: 计算几何
时间	   	: 2020-10-05 14:07
***************************************************************/
#ifndef __GEOMETRY_H__ 
#define __GEOMETRY_H__ 
#include <math.h>

#define EPSILON       (1e-12)  

/* 点与向量 */
typedef struct Point {
    double x, y;
} Point, Vector;

/* 线段与直线 */
typedef struct Segment {
    Point p1, p2;
} Segment, Line;

#define show_point(p)   ({   printf("x: %f, y: %f\n", (p).x, (p).y); })   

/* 圆 */
typedef struct Circle {
    Point c;    /* 圆心 */
    double r;   /* 半径 */
} Circle;

/* 多边形 */
typedef struct Polygon {
    Point *pts;     /* 多边形用点的序列来表示 */
} Polygon;

/* 向量运算 */

/* p1 + p2 */
#define vadd(p1, p2) ({  \
    Point __pt = {  \
        .x = (p1).x + (p2).x,   \
        .y = (p1).y + (p2).y,   \
    };\
    __pt;   })

/* p * k标量 */
#define vmul(p, k) ({  \
    Point __pt = {  \
        .x = (p).x * (k),   \
        .y = (p).y * (k),   \
    };\
    __pt;   })

/* p / k标量 */
#define vdiv(p, k)      ({  vmul( (p), 1.0 / (k) );   })

/* p1 - p2 */
#define vsub(p1, p2)    ({  vadd( (p1), vmul((p2), -1) );   })

/* p1 == p2 */
#define equals(a, b)   ({  fabs( (a) - (b) ) < EPSILON;  })
#define vequals(p1, p2)  ({  fabs( (p1).x - (p2).x ) < EPSILON && fabs( (p1).y - (p2).y ) < EPSILON;  })

/* p1 < p2 */
#define vless(p1, p2)  ({  (p1).x != (p2).x ? (p1).x < (p2).x : (p1).y < (p2).y;  })

/* 范数 与 大小(绝对值) */
#define vnorm(p)         ({  (p).x * (p).x + (p).y * (p).y;  })
#define vabs(p)          ({  sqrt(vnorm( (p) ));  })

/* 内积 */
#define vdot(a, b)         ({  (a).x * (b).x + (a).y * (b).y;  })
/* 外积, 返回表示向量 a x b大小的实数 */
#define vcross(a, b)       ({  (a).x * (b).y - (a).y * (b).x;  })

#endif	/* !__GEOMETRY_H__ */