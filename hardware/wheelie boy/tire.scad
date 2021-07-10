use <Toysans-Xe5d.ttf>

print = false;

$fn = print ? 200 : 100;

hub_width = 26;
hub_diam = 51.6;

tire_diam = 110;
tire_width = 50;
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
    letter(173, "o");
    letter(194, "y");
}

module rubber()
{
    minkowski()
    {
        cylinder(r=(tire_diam / 2) - tire_soften, h=tire_width - (tire_soften * 2));
        sphere(r=tire_soften);
    }
}

module tire()
{
    color([0.5, 0.5, 0.5]) difference() {
        union() {
            translate([0, 0, tire_soften]) rubber();
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
