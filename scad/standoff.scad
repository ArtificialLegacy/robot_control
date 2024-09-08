$fn=64;

height=35;
diameter=10;
holed=3.5;
nutOffset=7;
nut_dim=6.4;
nut_height=6;
nut_cutout=5.6;

difference() {
    cylinder(h=height, d=diameter);
    
    // screw hole
    cylinder(h=height, d=holed);
    
    // nut cutouts
    translate([0, 0, nutOffset]) {
        cylinder( d=nut_dim, h=nut_height, $fn=6, center=true);
    }
    translate([0, 0, height-nutOffset]) {
        cylinder( d=nut_dim, h=nut_height, $fn=6, center=true);
    }
    
    // nut slots
    translate([2.5, 0, nutOffset]) {
        cube([nut_cutout, nut_cutout, nut_height], center=true);
    }
    
    // nut slots
    translate([2.5, 0, height-nutOffset]) {
        cube([nut_cutout, nut_cutout, nut_height], center=true);
    }
}