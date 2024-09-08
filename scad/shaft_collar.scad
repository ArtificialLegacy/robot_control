include <BOSL2/std.scad>

use <./lib/shaft_lock.scad>

$fn=64;

thickness=4;
width=2;

cornerRadius=2;

lockWidth=4;
lockHeight=7;
cutoutWidth=1;

widthTolerance=-0.2;
heightTolerance=0.2;

difference() {
    // Main body
    union() {
        cuboid(
            [lockWidth+(width*2), lockHeight+(width*2), thickness],
            rounding=cornerRadius,
            edges=[FWD+RIGHT,BACK+RIGHT,FWD+LEFT,BACK+LEFT]
        );
        rotate([0, 0, -90]) {
            cuboid(
                [lockWidth+(width*2), lockHeight+(width*2), thickness],
                rounding=cornerRadius,
                edges=[FWD+RIGHT,BACK+RIGHT,FWD+LEFT,BACK+LEFT]
            );
        }
    }
    
    shaftlock(thickness, lockWidth, lockHeight, widthTolerance, heightTolerance);
    
    // Clamp Cutout
    translate([0, (lockHeight/2)+width/2, 0])
        cube([cutoutWidth, width+1, thickness+1], center=true);
}