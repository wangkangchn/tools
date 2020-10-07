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
#include <stdbool.h>
#include "tools.h"

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
    Point _pt = {  \
        .x = (p).x * (k),   \
        .y = (p).y * (k),   \
    };\
    _pt;   })

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

/* 点线关系 */
typedef enum LOCATION {
    COUNTER_CLOCKWISE = 1,
    CLOCKWISE = -1,
    ONLINE_BACK = 2,
    ONLINE_FRONT = -2,
    ON_SEGMENT = 0,
} LOCATION;


/**
 * ccw - 判断 p0, p1, p2 三个点的顺序
 * @p0, p1, p2: 待计算点	
 * @return: 
 *       p0, p1, p2 成顺时针, 返回 CLOCKWISE
 *       p0, p1, p2 成逆时针, 返回 COUNTER_CLOCKWISE
 *       p2, p0, p1 依次排列在同一条直线上(即 p2 位于 线段p0p1的左后方), 返回 ONLINE_BACK
 *       p0, p1, p2 依次排列在同一条直线上(即 p2 位于 线段p0p1的右前方), 返回 ONLINE_FRONT
 *       p0, p2, p1 依次排列在同一条直线上(即 p2 位于 线段p0p1的内部), 返回 ON_SEGMENT
 */
static inline LOCATION ccw(Point p0, Point p1, Point p2)
{
    Vector v01 = vsub(p1, p0);
    Vector v02 = vsub(p2, p0);

    if (vcross(v01, v02) > EPSILON)             return COUNTER_CLOCKWISE;
    else if (vcross(v01, v02) < -1 * EPSILON)   return CLOCKWISE;
    else if (vdot(v01, v02) < -1 * EPSILON)     return ONLINE_BACK;
    else if (vabs(v01) < vabs(v02))             return ONLINE_FRONT;
    
    return ON_SEGMENT;
}


/**
 * intersect - 判断两直线是否相交
 * @l1:     第一条直线	
 * @l2:     第二条直线	
 * @return: 相交返回 true, 不相交返回 false
 */
static inline bool intersect(Line l1, Line l2)
{   
    /* < 0 说明点位于直线的两侧, == 0 说明点位于直线上 */
    return (
        (ccw(l1.p1, l1.p2, l2.p1) * ccw(l1.p1, l1.p2, l2.p2) <= 0) &&
        (ccw(l2.p1, l2.p2, l1.p1) * ccw(l2.p1, l2.p2, l1.p2) <= 0)
    );
}

/* 距离计算 */

/**
 * distance_pp - 计算两点间的距离
 * @p1:     第一个点	
 * @p2:     第二个点	
 * @return: 两点间的距离
 */
static inline double distance_pp(Point p1, Point p2) { return vabs(vsub(p2, p1)); }

/**
 * distance_pl - 计算点与直线间的距离
 * @p:     点	
 * @l:     直线	
 * @return: 点与直线间的距离
 */
static inline double distance_pl(Point p, Line l)
{
    Vector vl = vsub(l.p2, l.p1);   /* 直线 l 的方向向量 */
    Vector vpl = vsub(p, l.p1);    /* 向量 pp1 */
    return fabs(vcross(vl, vpl) / vabs(vl));
}

/**
 * distance_ps - 计算点与线段间的距离
 * @p:     点	
 * @s:     线段	
 * @return: 点与线段间的距离
 */
static inline double distance_ps(Point p, Segment s)
{
    /* Note: 注意要同一起点!!! */
    if (vdot(vsub(s.p2, s.p1), vsub(p, s.p1)) < 0)          return distance_pp(p, s.p1);
    else if (vdot(vsub(s.p1, s.p2), vsub(p, s.p2)) < 0)     return distance_pp(p, s.p2);
    else                                                    return distance_pl(p, s);
}

/**
 * distance_ss - 计算线段与线段间的距离
 *      线段间的距离为:
 *  1. 相交, 距离为0
 *  2. s1.p1 与 s2 的距离, s1.p2 与 s2 的距离, s2.p1 与 s1 的距离, s2.p2 与 s1 的距离的最小值 
 * @s1:     线段	
 * @s2:     线段	
 * @return: 线段与线段间的距离
 */
static inline double distance_ss(Segment s1, Segment s2)
{
    if (!intersect(s1, s2)) {
        return min(
                min(distance_ps(s1.p1, s2), distance_ps(s1.p2, s2)), 
                min(distance_ps(s2.p1, s1), distance_ps(s2.p2, s1))
                );
    }
    return 0.0;
}

#endif	/* !__GEOMETRY_H__ */