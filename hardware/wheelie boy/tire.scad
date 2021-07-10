use <Toysans-Xe5d.ttf>

print = false;

$fn = print ? 200 : 50;

hub_width = 26;
hub_diam = 51.6;

tire_diam = 110;
tire_width = 40;
tire_soften = 4;


module hub()
{
    cylinder(r=hub_diam / 2, h=hub_width);
}

module letter(rotation, letter)
{
    font_size = (tire_diam - hub_diam) / 4;
    tire_centre = (hub_diam / 2) + ((tire_diam - hub_diam) / 4);
    rotate([0, 0, -rotation]) translate([0, tire_centre - (font_size / 2), -1]) linear_extrude(2) text(letter, font_size, "ToySans", halign="center");
}

module model()
{   
    letter(0, "W");
    letter(25, "h");
    letter(45, "e");
    letter(64, "e");
    letter(81, "l");
    letter(94, "i");
    letter(110, "e");
    letter(150, "B");
    letter(172, "o");
    letter(193, "y");
}

module rubber()
{
    minkowski()
    {
        cylinder(r=(tire_diam / 2) - tire_soften, h=tire_width - (tire_soften * 2));
        sphere(r=tire_soften);
    }
}

module tread()
{
    depth = 4;
    solid_centre = 5;
    width = 10;
    cuts = 18;
    angle = 360 / cuts;
    for (i = [0:cuts])
    {
        rotate([0, 0, i * angle]) translate([-width/2, (tire_diam / 2) - depth, -tire_soften-1]) cube([5, depth * 2, (tire_width / 2) + 1 - (solid_centre/2)]);
    }
    
    for (i = [0:cuts])
    {
        rotate([0, 0, (i * angle) + (angle / 2)]) translate([-width/2, (tire_diam / 2) - depth, (tire_width / 2) + (solid_centre/2) - tire_soften]) cube([5, depth * 2, tire_width / 2]);
    }
}

module tire()
{
    color([0.5, 0.5, 0.5]) difference() {
        union() {
            translate([0, 0, tire_soften]) difference() {
                rubber();
                tread();
            }
            translate([0, 0, tire_width]) model();
        }
        
        // Hub hole.
        translate([0, 0, -10]) cylinder(r=hub_diam / 2, h=tire_width + 20);
    }
}

if (!print)
{
    hub();
}    

tire();
