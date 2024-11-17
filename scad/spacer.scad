
use <./lib/shaft_lock.scad>

$fa=1;
$fs=0.5;

length=4;

type="free";

innerDiameter=8;
outerDiameter=10;

lockWidth=4;
lockHeight=7;
shaftTolerance=0.2;

difference() {
    cylinder(h=length, r=outerDiameter/2);
    
    if (type == "free") {
        translate([0,0,-0.5])
            cylinder(h=length+1, d=innerDiameter+shaftTolerance);
    } else if (type == "lock") {
        translate([0,0,length/2]) {
            shaftlock(length, lockWidth, lockHeight, shaftTolerance, shaftTolerance);
        }
    }
}