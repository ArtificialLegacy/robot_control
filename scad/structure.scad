include <BOSL2/std.scad>

$fn=64;

thickness = 5;
holeLength = 5;
holeWidth = 2;

cornerRadius = 2;
edgeRadius = 2;

sideHoleWidth = 2;

type = "plate";
uChannel = false;
uSideLength = 2;

if (type == "plate") {
    if (uChannel) {
        union() {
            faceUFront([BACK+LEFT, BACK+RIGHT]);
            faceUBack([BACK+LEFT, BACK+RIGHT]);
            
            face(holeWidth, holeLength, []);
        }
    } else {
        face(holeWidth, holeLength, [FWD+RIGHT, BACK+RIGHT, BACK+LEFT, FWD+LEFT]);
    }
} else if (type == "angle") {
    union() {
        if (uChannel) {
            faceUFront([BACK+RIGHT]);
            faceUBack([BACK+RIGHT]);
            
            face(holeWidth, holeLength, []);
            faceLeft([]);
            
            translate([-thickness, -thickness, 0]) {
                connectorCorner([BOT+LEFT, BOT+FRONT, LEFT+FRONT]);
            }
            translate([-thickness, holeLength*10, 0]) {
                connectorCorner([BOT+LEFT, BOT+BACK, LEFT+BACK]);
            }
        } else {
            face(holeWidth, holeLength, [FWD+RIGHT, BACK+RIGHT]);
            faceLeft([FWD+RIGHT, BACK+RIGHT]);
        }
    }
} else if (type == "c") {
    union() {
        face(holeWidth, holeLength, []);

        if (uChannel) {
            faceUFront([]);
            faceUBack([]);
            
            faceLeft([]);
            faceRight([]);
            
            translate([-thickness, -thickness, 0]) {
                connectorCorner([BOT+LEFT, BOT+FRONT, LEFT+FRONT]);
            }
            translate([-thickness, holeLength*10, 0]) {
                connectorCorner([BOT+LEFT, BOT+BACK, LEFT+BACK]);
            }
            translate([holeWidth*10, -thickness, 0]) {
                connectorCorner([BOT+FRONT, BOT+RIGHT, RIGHT+FRONT]);
            }
            translate([holeWidth*10, holeLength*10, 0]) {
                connectorCorner([BOT+RIGHT, BOT+BACK, RIGHT+BACK]);
            }
        } else {
            faceLeft([FWD+RIGHT, BACK+RIGHT]);
            faceRight([FWD+RIGHT, BACK+RIGHT]);
        }
    }
} else if (type == "tube") {
    union() {
        face(holeWidth, holeLength, []);
        faceLeft([]);
        faceRight([]);
        faceTop();

        if (uChannel) {
            faceUFront([]);
            faceUBack([]);

            translate([0, -thickness, thickness+uSideLength*10]) {
                connectorU([FRONT+TOP]);
            }
            translate([0, holeLength*10, thickness+uSideLength*10]) {
                connectorU([BACK+TOP]);
            }
            
            translate([-thickness, -thickness, 0]) {
                connectorCornerTall([BOT+LEFT, BOT+FRONT, LEFT+FRONT, TOP+LEFT, TOP+FRONT]);
            }
            translate([-thickness, holeLength*10, 0]) {
                connectorCornerTall([BOT+LEFT, BOT+BACK, LEFT+BACK, TOP+LEFT, TOP+BACK]);
            }
            translate([holeWidth*10, -thickness, 0]) {
                connectorCornerTall([BOT+FRONT, BOT+RIGHT, RIGHT+FRONT, TOP+RIGHT, TOP+FRONT]);
            }
            translate([holeWidth*10, holeLength*10, 0]) {
                connectorCornerTall([BOT+RIGHT, BOT+BACK, RIGHT+BACK, TOP+RIGHT, TOP+BACK]);
            }
        }
    }
}

module connector(e) {
    cuboid(
        [thickness, holeLength*10, thickness],
        anchor=[-1,-1,-1],
        rounding=edgeRadius,
        edges=e
    );
}

module connectorU(e) {
    cuboid(
        [holeWidth*10, thickness, thickness],
        anchor=[-1, -1, -1],
        rounding=edgeRadius,
        edges=e
    );
}

module connectorCorner(e) {
    cuboid(
        [thickness, thickness, sideHoleWidth*10+thickness],
        anchor=[-1, -1, -1],
        rounding=edgeRadius,
        edges=e
    );
}

module connectorCornerTall(e) {
    cuboid(
        [thickness, thickness, sideHoleWidth*10+thickness*2],
        anchor=[-1, -1, -1],
        rounding=edgeRadius,
        edges=e
    );
}

module faceLeft(e) {
    translate([-thickness, 0, 0]) {
        connector([BOT+LEFT]);
    }

    rotate([0, -90, 0])
    translate([thickness, 0, 0])
    {
        face(sideHoleWidth, holeLength, e);
    }
}

module faceRight(e) {
    translate([holeWidth*10, 0, 0]) {
        connector([BOT+RIGHT]);
    }
    rotate([0, -90, 0])
    translate([thickness, 0, -(holeWidth*10)-thickness])
    {
        face(sideHoleWidth, holeLength, e);
    }
}

module faceTop() {
    translate([-thickness, 0, sideHoleWidth*10+thickness]) {
        connector([TOP+LEFT]);
    }
    translate([holeWidth*10, 0, sideHoleWidth*10+thickness]) {
        connector([TOP+RIGHT]);
    }
    rotate([0, 0, 0])
    translate([0, 0, (holeWidth*10)+thickness])
    {
        face(sideHoleWidth, holeLength, []);
    }
}

module faceUFront(e) {
    translate([0, -thickness, 0]) {
        connectorU([FRONT+BOT]);
    }
    rotate([90, 0, 0]) 
    translate([0, thickness, 0]) {
        face(holeWidth, uSideLength, e);
    }
}

module faceUBack(e) {
    translate([0, holeLength*10, 0]) {
        connectorU([BACK+BOT]);
    }
    rotate([90, 0, 0]) 
    translate([0, thickness, (-holeLength*10)-thickness]) {
        face(holeWidth, uSideLength, e);
    } 
}

module face(width, length, e) {
    difference() {
        cuboid(
            [width*10, length*10, thickness],
            anchor=[-1,-1,-1],
            rounding=cornerRadius,
            edges=e
        );
        
        for (x = [0:1:length-1]) {
            for (y = [0:1:width-1]) {
                translate([5 + 10*y, 5 + 10*x, thickness/2])
                    cylinder(h=thickness+1, r=1.75, center=true);
            }
        }
        
        for (x = [0:1:length-2]) {
            for (y = [0:1:width-2]) {
                translate([10 + 10*y, 10 + 10*x, thickness/2])
                    cylinder(h=thickness+1, r=4, center=true);
            }
        }
    }
}