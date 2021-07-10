$fn = 50;

module rod(diameter, height) {
    color([1, 0, 0]) cylinder(h=height, r=diameter / 2);
}

module base(diameter) {
    color([1, 0, 0]) sphere(r=diameter / 2);
}

module screen(width, height, thickness) {
    color([0, 0, 0]) translate([-thickness / 2, -width / 2, 0]) cube([thickness, width, height]);
}

module wheel(width, diameter) {
    
    translate([0, 0, -width * 0.125]) cylinder(h=width * 0.25, r=(diameter / 2) - width);
    color([0.2, 0.2, 0.2]) rotate_extrude() translate([(diameter / 2) - (width / 2), 0, 0]) circle(r = width / 2);
}

ball_diam = 10;
rod_height = 35;
rod_diam = 1;

screen_height = ball_diam * 2;
screen_width = screen_height * (3/4);

base(ball_diam);
translate([0, 0, ball_diam / 2]) rod(rod_diam, rod_height);
translate([0, 0, rod_height + (ball_diam / 2)]) screen(screen_width, screen_height, 1);
translate([0, (ball_diam / 2)+1.5, 0]) rotate([-90, 0, 0]) wheel(ball_diam / 2.5, ball_diam * 1.2);
translate([0, -(ball_diam / 2)-1.5, 0]) rotate([90, 0, 0]) wheel(ball_diam / 2.5, ball_diam * 1.2);
