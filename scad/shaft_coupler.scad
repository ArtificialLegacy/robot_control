
$fa=1;
$fs=0.5;

shaft1Width=4;
shaft1Height=7;
tolerance1=0.4;
d1=14;
thickness1=4;
shaft1Double=true;

shaft2Width=3.7;
shaft2Height=5.2;
tolerance2=0.2;
d2=14;
thickness2=6;
shaft2Double=false;

centerThickness=2;
centerd=9;

union() {
    difference() {
        cylinder(h=thickness1, d=d1, center=true);
        cube(
            [shaft1Width+tolerance1, shaft1Height+tolerance1, thickness1],
            center=true
        );
        if (shaft1Double) {
            rotate([0, 0, 90]) {
                cube(
                    [shaft1Width+tolerance1, shaft1Height+tolerance1, thickness1],
                    center=true
                );
            }
        }
    }
    translate([0, 0, thickness1/2]) {
        cylinder(h=centerThickness, d=centerd, center=true);
    }
    translate([0, 0, thickness1/2+centerThickness]) {
        difference() {
            cylinder(h=thickness2, d=d2, center=true);
            cube(
                [shaft2Width+tolerance2, shaft2Height+tolerance2, thickness2],
                center=true
            );
            if (shaft2Double) {
                rotate([0, 0, 90]) {
                    cube(
                        [shaft2Width+tolerance2, shaft2Height+tolerance2, thickness2],
                        center=true
                    );
                }
            }
        }
    }
}