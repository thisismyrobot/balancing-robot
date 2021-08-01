use <Toysans-Xe5d.ttf>

print = false;

$fn = print ? 200 : 20;

hub_width = 26;
hub_diam = 51.6;

hubcap_thickness = 1;

spoke_width = 0.75; // Tune to single pass of nozzle.

rim_width = 2;
tire_diam = 110;
tire_width = 30;
    

module hub()
{
    cylinder(r=hub_diam / 2, h=hub_width);
}

module hubCap()
{
    color([0.5, 0.5, 0.5]) cylinder(r=hub_diam / 2, h=hubcap_thickness);
    
    font_size = hub_diam / 6;
    translate([0, 0, hubcap_thickness]) linear_extrude(hubcap_thickness) text("Wheelie", font_size, "ToySans", halign="center", spacing=1.1);    
    translate([0, -hub_diam / 4.5, hubcap_thickness]) linear_extrude(hubcap_thickness) text("Boy", font_size, "ToySans", halign="center", spacing = 1.1);    
}

module tire()
{
    // Inner rim.
    difference() {
        cylinder(r=(hub_diam / 2) + rim_width, h=tire_width);
        translate([0, 0, -10]) cylinder(r=hub_diam / 2, h=tire_width + 20);
    }

    // Inner rim.
    difference() {
        cylinder(r=(tire_diam / 2), h=tire_width);
        translate([0, 0, -10]) cylinder(r=(tire_diam / 2) - rim_width, h=tire_width + 20);
    }
   
    // Airless-ness (think Michelin "Uptis")
    length = ((tire_diam - hub_diam) / 2) - (rim_width * 1.5);
    spokes = 30;
    for ( i = [0 : spokes] )
    {
        rotate([0, 0, (360 / spokes) * i]) translate([(hub_diam / 2) + rim_width, 0, 0]) linear_extrude(tire_width) {
            polygon(points=[[-rim_width/2,-spoke_width/2],[length,-spoke_width/2],[length,spoke_width/2],[-rim_width/2,spoke_width/2]]);
        }
    }
}

if (!print) {
    hub();
    translate([0, 0, hub_width]) hubCap();
}
else {
    translate([0, ((tire_diam + hub_diam) / 2) + 10, 0]) hubCap();
}

color([0.5, 0.5, 0.5]) tire();
