include <BOSL2/std.scad>

use <./lib/shaft_lock.scad>

$fn=64;

thickness=2;
edgeRadius=2;
type="bearing";
bearingDiameter=7;
lockWidth=4;
lockHeight=7;
shaftTolerance=0.2;

difference() {
    cuboid(
        [16, 16, thickness],
        anchor=[-1,-1,-1],
        rounding=edgeRadius,
        edges=[FWD+RIGHT,BACK+RIGHT,BACK+LEFT,FWD+LEFT]
    );
    
    if (type == "bearing") {
        translate([8, 8, thickness/2]) {
            cylinder(
                h=thickness,
                r=bearingDiameter/2+shaftTolerance,
                center=true
            );
        }
    } else if (type == "lock") {
        translate([8, 8, thickness/2]) {
            shaftlock(thickness, lockWidth, lockHeight, shaftTolerance, shaftTolerance);
        }
    }
    
    // screw holes
    translate([3, 3, thickness/2])
        cylinder(h=thickness+1, r=1.75, center=true);
    translate([13, 3, thickness/2])
        cylinder(h=thickness+1, r=1.75, center=true);
    translate([3, 13, thickness/2])
        cylinder(h=thickness+1, r=1.75, center=true);
    translate([13, 13, thickness/2])
        cylinder(h=thickness+1, r=1.75, center=true);
        
    // shaft holes
    translate([8, -2, thickness/2])
        cylinder(h=thickness+1, d=8, center=true);
    translate([-2, 8, thickness/2])
        cylinder(h=thickness+1, d=8, center=true);
    translate([8, 18, thickness/2])
        cylinder(h=thickness+1, d=8, center=true);
    translate([18, 8, thickness/2])
        cylinder(h=thickness+1, d=8, center=true);
}