include <BOSL2/std.scad>

$fn=64;

length=60;
width=3.8;
height=6.8;
edgeRadius=1;

cuboid(
    [width, height, length],
    rounding=edgeRadius,
    edges=[FWD+RIGHT,BACK+RIGHT,FWD+LEFT,BACK+LEFT]
);

rotate([0, 0, 90]) {
    cuboid(
        [width, height, length],
        rounding=edgeRadius,
        edges=[FWD+RIGHT,BACK+RIGHT,FWD+LEFT,BACK+LEFT]
    );
}