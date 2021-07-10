use <Toysans-Xe5d.ttf>

print = false;

$fn = print ? 250 : 50;

hub_width = 26;
hub_diam = 51.6;

tire_diam = 110;
tire_width = 50;
tire_soften = 5;


module hub()
{
    cylinder(r=hub_diam / 2, h=hub_width);
}

module model()
{   
    letters = "Wheelie Boy";
    chars = len(letters);
    font_size = (tire_diam - hub_diam) / 4;
    tire_centre = (hub_diam / 2) + ((tire_diam - hub_diam) / 4);
    
    for (c =[0:chars])
    {
        rotate([0, 0, -c*15]) translate([0, tire_centre, 0]) linear_extrude(1) text(letters[c], font_size, "ToySans", valign="center", halign="center");
    }

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
