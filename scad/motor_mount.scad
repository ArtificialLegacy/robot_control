include <BOSL2/std.scad>

$fa=1;
$fs=0.5;

cornerRadius = 2;
thickness = 2;

nutOffset=7;
nut_dim=6.4;
nut_height=6;
nut_cutout=5.6;

difference() {
        union() {
            cuboid(
                [20, 40, thickness],
                anchor=[-1,-1,-1],
                rounding=cornerRadius,
                edges=[FWD+RIGHT, BACK+RIGHT, BACK+LEFT, FWD+LEFT]
            );
            
            translate([1.5, 30, 0]) {
                cylinder(h=thickness+16, d=10);
            }
            translate([18.5, 30, 0]) {
                cylinder(h=thickness+16, d=10);
            }

            translate([-10, 30, 0]) {
                cuboid(
                    [40, 10, thickness],
                    anchor=[-1,-1,-1],
                    rounding=cornerRadius,
                    edges=[FWD+RIGHT, BACK+RIGHT, BACK+LEFT, FWD+LEFT]
                );
            }
        }
        
        shaftHoles([10, 10, 0]);
        
        // motor screw holes
        translate([1.5, 30, 0])
            cylinder(h=thickness+16, d=3.5);
        translate([18.5, 30, 0])
            cylinder(h=thickness+16, d=3.5);

        // nut cutouts
        translate([1.5, 30, (thickness+16)-nutOffset]) {
            cylinder( d=nut_dim, h=nut_height, $fn=6, center=true);
        }
        translate([18.5, 30, (thickness+16)-nutOffset]) {
            cylinder( d=nut_dim, h=nut_height, $fn=6, center=true);
        }
        // nut slots
        translate([1.5, 30+2.5, (thickness+16)-nutOffset]) {
            cube([nut_cutout, nut_cutout, nut_height], center=true);
        }
        translate([18.5, 30+2.5, (thickness+16)-nutOffset]) {
            cube([nut_cutout, nut_cutout, nut_height], center=true);
        }
    }

module shaftHoles(pos) {
    // shaft hole
    translate(pos)
        cylinder(h=thickness, d=8);
        
    // screw holes
    translate([pos.x-5, pos.y-5, 0])
        cylinder(h=thickness, d=3.5);
    translate([pos.x+5, pos.y-5, 0])
        cylinder(h=thickness, d=3.5);
    translate([pos.x-5, pos.y+5, 0])
        cylinder(h=thickness, d=3.5);
    translate([pos.x+5, pos.y+5, 0])
        cylinder(h=thickness, d=3.5);
        
    // top screw holes
    translate([pos.x-15, pos.y+25, 0])
        cylinder(h=thickness, d=3.5);
    translate([pos.x+15, pos.y+25, 0])
        cylinder(h=thickness, d=3.5);
}