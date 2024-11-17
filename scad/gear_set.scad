
include <BOSL2/std.scad>
include <BOSL2/gears.scad>
use <./lib/shaft_lock.scad>

$fa=1;
$fs=0.5;

height=5;

spacingx=1;
spacingy=0;

teeth1=7;
teeth2=5;
mod=3;

lockWidth=4;
lockHeight=7;
shaftTolerance=0.2;

gear1=true;

pressure=20;

space=sqrt(pow(spacingx*10, 2) + pow(spacingy*10, 2));

sdist=gear_dist(mod=mod, teeth1, teeth2, pressure_angle=pressure);
pshift=get_profile_shift(round(sdist/space)*space, teeth1, teeth2, mod=mod, pressure_angle=pressure);
ps1=pshift/2;
ps2=pshift/2;

shorten=gear_shorten(teeth1, teeth2, 0, ps1, ps2, pressure_angle=pressure);

dist=gear_dist(mod=mod, teeth1, teeth2, 0, ps1, ps2, pressure_angle=pressure);

pr1=(0.3183*(teeth1-2.7)*(PI*mod))/2;
pr2=(0.3183*(teeth2-2.7)*(PI*mod))/2;

gridRadius1=ceil(pr1/10)*10;
gridSize1=ceil(pr1/5);
gridRadius2=ceil(pr2/10)*10;
gridSize2=ceil(pr2/5);

if (gear1) {
    gear(teeth1, ps1, gridSize1, gridRadius1, pr1);
} else {
    translate([dist, 0, 0]) {
        gear(teeth2, ps2, gridSize2, gridRadius2, pr2);
    }
}

module gear(teeth, ps, gs, gr, pr, spin=0, hide=0) {
    difference() {
        union() {
            spur_gear(mod=mod, teeth=teeth, thickness=height, profile_shift=ps, shorten=shorten, spin=spin, pressure_angle=pressure, hide=hide);
            translate([0, 0, -height/2]) {
                cylinder(h=height, d=10);
            }
        }

        shaftlock(height, lockWidth, lockHeight, shaftTolerance, shaftTolerance);

        for (x = [0:1:gs]) {
            for (y = [0:1:gs]) {
                py = 5 + 10*y - gr;
                px = 5 + 10*x - gr;
                if (pow(px, 2) + pow(py, 2) < pow(pr-1.75, 2)) {
                    translate([py, px, 0]) {
                        cylinder(h=height, r=1.75, center=true);
                    }
                }
            }
        }
    }
}