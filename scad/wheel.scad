
include <BOSL2/std.scad>
use <./lib/shaft_lock.scad>

$fa=1;
$fs=0.5;

diameter=50;
height=10;

lockWidth=4;
lockHeight=7;
shaftTolerance=0.2;

tread=true;
treadThickness=1;
treadCount=10;
treadRatio=1.5;

gridRadius=ceil(diameter/2/10)*10;
gridSize=ceil(diameter/10);

difference() {
    union() {
        cylinder(h=height, d=diameter);

        if (tread) {
            for (i = [0:1:treadCount]) {
                rotate([0, 0, (360/(treadCount*2))*(i*2)]) {
                    pie_slice(ang=(360/treadCount)/treadRatio, r=diameter/2+treadThickness, h=height);
                }
            }
        }
    }

    translate([0, 0, height/2]) {
        shaftlock(height, lockWidth, lockHeight, shaftTolerance, shaftTolerance);
    }

    // screw holes
    for (x = [0:1:gridSize]) {
        for (y = [0:1:gridSize]) {
            py = 5 + 10*y - gridRadius;
            px = 5 + 10*x - gridRadius;
            if (pow(px, 2) + pow(py, 2) < pow(diameter/2-1.75, 2)) {
                translate([py, px, height/2]) {
                    cylinder(h=height, r=1.75, center=true);
                }
            }
        }
    }
}